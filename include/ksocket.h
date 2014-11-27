/**
 * 	ksocket.h
 *	copyright (c) proguru
 *	Sep. 03,2008
 *	multithread block socket wrap for TCP/UDP comm
 */

#ifndef __KSOCKET_H__
#define __KSOCKET_H__

#include "winsock2.h"
#include "kthread.h"
#include <map>
#include <list>
#pragma comment(lib,"Ws2_32.lib")

#define WM_EX_EXIT	WM_APP+1

class SockInit{
public:
	SockInit();
	~SockInit();
private:
	static int m_s_count;
};

class SockAddrIn : public sockaddr_in{
public:
	SockAddrIn(unsigned short port,char* ip_or_name=0);
	inline operator sockaddr_in(){return *this;}
	inline operator sockaddr_in*(){return this;}
private:
	ULONG GetIPAddress(char* ip_or_name);
};

class KSocket{
public:
	KSocket(SOCKET s);
	KSocket(int protocol=IPPROTO_TCP,int type=SOCK_STREAM,int af=AF_INET);

	inline operator SOCKET(){return m_socket;}
	inline operator bool(){return m_socket!=INVALID_SOCKET;}
	inline KSocket& operator=(SOCKET s){this->m_socket=s;return *this;}

	int Send(char* buf,int len,int flags=0);		//TCP
	int Receive(char* buf,int len,int flags=0);
	int SendTo(char* buf,int len,sockaddr_in* to,int flags=0);		//UDP
	int ReceiveFrom(char* buf,int len,sockaddr_in* from,int flags=0);

	//
	int Bind(sockaddr_in* name);
	int Bind(unsigned short port,char* ip=0);
	int Listen(int nMaxConnections=SOMAXCONN);
	SOCKET Accept(sockaddr_in* name);
	SOCKET Accept(void);
	SOCKET Connect(sockaddr_in* name);
	
	//
	int Shutdown(int how=SD_BOTH);
	int Close(void);

	//
	int SetOption(int nOptName,const char* szOptVal,int nOptLen,int nOptLevel=SOL_SOCKET);
	int GetOption(int nOptName,char* szOptVal,int& nOptLen,int nOptLevel=SOL_SOCKET);

	//
	unsigned short GetLocalPort(void);
	unsigned short GetRemotePort(void);
	int GetLocalAddr(sockaddr_in* pAddr);
	int GetRemoteAddr(sockaddr_in* pAddr);
private:
	SOCKET 		m_socket;
};

template<typename T,typename TBase=KThread>
class KCommHandler : public KThreadImpl<T,TBase>{
public:
	KCommHandler(KSocket& socket):m_socket(socket){}
protected:
	KSocket	m_socket;
};

struct KClientCommand{
	virtual ~KClientCommand()=0;
	virtual void Execute(KSocket& s)=0;
};

class KClientCommHandler: public KCommHandler<KClientCommHandler>{
public:
	KClientCommHandler(KSocket& s):KCommHandler(s){}
	unsigned int KCALLBACK Execute(void* pParam){
		MSG msg;
		while(::GetMessage(&msg,0,0,0)){
			printf("receive command\n");
			KClientCommand* pClientCmd=(KClientCommand*)(msg.wParam);
			pClientCmd->Execute(m_socket);	
		}
		//close socket
		m_socket.Shutdown();
		m_socket.Close();
		delete this;
		return 0;
	}
};

struct KServerCommand{
	virtual ~KServerCommand()=0;
	virtual void Execute(KSocket& s,const char* szCmdParameter)=0;
};

typedef std::map<const char*,KServerCommand*> 	SrvCmdMap_t;
typedef std::pair<const char*,KServerCommand*>	SrvCmdPair_t;

class KServerCommHandler : public KCommHandler<KServerCommHandler>{
public:
	
	KServerCommHandler(KSocket& s,SrvCmdMap_t& SrvCmdMap)
		:KCommHandler(s),m_mapSrvCmd(SrvCmdMap){
			memset(m_szRecvBuf,0,128);
	}
	unsigned int KCALLBACK Execute(void* pParam){
		int nRecv=0;
		for(;;){
			nRecv=m_socket.Receive(m_szRecvBuf,128);	
			
			if(nRecv==0 || nRecv==SOCKET_ERROR){
				printf("client disconnected!\n");
				return -1;
			}
			m_szRecvBuf[nRecv]='\0';
			SrvCmdMap_t::iterator iter;
			for(iter=m_mapSrvCmd.begin();iter!=m_mapSrvCmd.end();iter++){
				if(!strncmp(m_szRecvBuf,iter->first,4)){
					iter->second->Execute(m_socket,m_szRecvBuf);
				}
			}	
		}
		delete this;
		return 0;
	}
private:
	char		m_szRecvBuf[128];
	SrvCmdMap_t	m_mapSrvCmd;
};

class KTCPAcceptor{
public:
	KTCPAcceptor(unsigned short port,char* ip_or_name=0);
	bool Open();
	KSocket Accept();
private:
	KSocket m_socket;
	unsigned short m_nPort;
	char* 	m_szIPOrName;
};

class KTCPConnector{
public:
	KTCPConnector(unsigned short port,char* ip_or_name);
	bool Open();
	KSocket Connect();
private:
	unsigned short 	m_nPort;
	char* 			m_szIPOrName;
	sockaddr_in		m_addrRemote;
	KSocket			m_socket;
};

template<typename handler_t,typename acceptor_t>
class KSocketAcceptor : public KThreadImpl<KSocketAcceptor<handler_t,acceptor_t>>{
public:
	KSocketAcceptor(acceptor_t& acceptor,SrvCmdMap_t& SrvCmdMap)
		:KThreadImpl(CREATE_SUSPENDED),m_acceptor(acceptor),m_mapSrvCmd(SrvCmdMap){}
	~KSocketAcceptor(){
		std::list<handler_t*>::iterator it;
		for(it=m_listHandler.begin();it!=m_listHandler.end();it++)
			delete *it;		
	}
	
	bool Open(){
		if(m_acceptor.Open()){
			ResumeThread();
			return true;
		}
		return false;	
	}
	//std::list<handler_t*>& GetHandlerList(){}
	unsigned int KCALLBACK Execute(void* pParam){
		KSocket peer(INVALID_SOCKET);
		for(;;){
			peer=m_acceptor.Accept();
			if(peer){
				printf("client connected!\n");
				handler_t* pHandler=new handler_t(peer,m_mapSrvCmd);
				::CloseHandle(*pHandler);
				m_listHandler.push_front(pHandler);
			}
		}
		return 0;
	}
private:
	acceptor_t 				m_acceptor;
	std::list<handler_t*> 	m_listHandler;
	SrvCmdMap_t				m_mapSrvCmd;
};

template<typename handler_t,typename connector_t>
class KSocketConnector : public KThreadImpl<KSocketConnector<handler_t,connector_t>>{
public:
	KSocketConnector(connector_t& connector)
		:KThreadImpl(CREATE_SUSPENDED),m_connector(connector),m_pHandler(0){}

	bool Open(){
		if(m_connector.Open()){
			ResumeThread();
			return true;
		}
		return false;	
	}
	handler_t* GetHandler(){return m_pHandler;}
	unsigned int KCALLBACK Execute(void* pParam){
		KSocket s(INVALID_SOCKET);
		s=m_connector.Connect();
		if(s){
			printf("connected to server!\n");
			m_pHandler=new handler_t(s);			

			HANDLE hHandler=*m_pHandler;
			::WaitForSingleObject(hHandler,INFINITE);
			::CloseHandle(hHandler);
		}	
		printf("connector exit!\n");
		return 0;
	}
private:
	connector_t	m_connector;
	handler_t*	m_pHandler;
}; 

template<typename connector_t>
class KSocketConnHandler : public KThreadImpl<KSocketConnHandler<connector_t>>{
public:
	KSocketConnHandler(connector_t& connector)
		:KThreadImpl(CREATE_SUSPENDED),m_connector(connector){}

	bool Open(){
		if(m_connector.Open()){
			ResumeThread();
			return true;
		}
		return false;	
	}

	unsigned int KCALLBACK Execute(void* pParam){
		MSG 	msg;
		KSocket s(INVALID_SOCKET);
		s=m_connector.Connect();
		if(s){
			while(::GetMessage(&msg,0,0,0)){
				printf("receive command\n");
				KClientCommand* pClientCmd=(KClientCommand*)(msg.wParam);
				pClientCmd->Execute(s);	
			}
			//close socket
			s.Shutdown();
			s.Close();
		}	
		printf("connector exit!\n");
		delete this;
		return 0;
	}
private:
	connector_t	m_connector;
}; 
#endif //__KSOCKET_H__
