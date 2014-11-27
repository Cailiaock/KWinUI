/**
 * 	ksocket.cpp
 *	copyright (c) proguru
 *	Sep. 03,2008
 *	multithread block socket wrap for TCP/UDP comm
 */

#include "ksocket.h"

int SockInit::m_s_count=0;
static SockInit socket_initialize;
//
SockInit::SockInit(){
	if(m_s_count++==0){
		WSADATA data;
		WSAStartup(MAKEWORD(2,2),&data);
	}
	printf("m_s_count=%i\n",m_s_count);
}
SockInit::~SockInit(){
	if(--m_s_count==0){
		WSACleanup();
	}
	printf("m_s_count=%i\n",m_s_count);
}
//SockAddrIn
SockAddrIn::SockAddrIn(unsigned short port,char* ip_or_name){
	memset(this,0,sizeof(SockAddrIn));

	sin_family	=AF_INET;
	sin_port	=::htons(port);

	if(ip_or_name)
		sin_addr.s_addr=GetIPAddress(ip_or_name);
	else
		sin_addr.s_addr=INADDR_ANY;
}
ULONG SockAddrIn::GetIPAddress(char* ip_or_name){
	hostent* pHostEntry;
	ULONG	uAddr=::inet_addr(ip_or_name);

	if(INADDR_NONE==uAddr){
		if(pHostEntry=::gethostbyname(ip_or_name))
			uAddr=*((ULONG *)pHostEntry->h_addr_list[0]);
	}
	return uAddr;
}

//KSocket
KSocket::KSocket(SOCKET s):m_socket(s){}

KSocket::KSocket(int protocol,int type,int af){
	m_socket=socket(af,type,protocol);
}

int KSocket::Send(char* buf,int len,int flags){
	assert(m_socket);
	return send(m_socket,buf,len,flags);
}		
int KSocket::Receive(char* buf,int len,int flags){
	assert(m_socket);
	return recv(m_socket,buf,len,flags);
}
int KSocket::SendTo(char* buf,int len,sockaddr_in* to,int flags){
	assert(m_socket);
	return sendto(m_socket,buf,len,flags,(sockaddr*)to,sizeof(sockaddr_in));
}		
int KSocket::ReceiveFrom(char* buf,int len,sockaddr_in* from,int flags){
	assert(m_socket);
	int nFromLen=sizeof(sockaddr_in);
	return recvfrom(m_socket,buf,len,flags,(sockaddr*)from,&nFromLen);
}
//
int KSocket::Bind(sockaddr_in* name){
	assert(m_socket);
	return bind(m_socket,(sockaddr*)name,sizeof(sockaddr_in));
}

int KSocket::Bind(unsigned short port,char* ip){
	assert(m_socket);
	SockAddrIn name(port,ip);
	return Bind(&name);
}
int KSocket::Listen(int nMaxConnections){
	assert(m_socket);
	return listen(m_socket,nMaxConnections);
}

SOCKET KSocket::Accept(sockaddr_in* name){
	assert(m_socket);
	int nNameLen=sizeof(sockaddr_in);
	return accept(m_socket,(sockaddr*)name,&nNameLen);
}
SOCKET KSocket::Accept(void){
	assert(m_socket);
	return accept(m_socket,0,0);
}
SOCKET KSocket::Connect(sockaddr_in* name){
	assert(m_socket);
	if(connect(m_socket,(sockaddr*)name,sizeof(sockaddr_in))==0)
		return m_socket;
	return INVALID_SOCKET;
}

//
int KSocket::Shutdown(int how){
	assert(m_socket);
	return shutdown(m_socket,how);
}
int KSocket::Close(void){
	assert(m_socket);
	return closesocket(m_socket);
}
int KSocket::SetOption(int nOptName,const char* szOptVal,int nOptLen,int nOptLevel){
	assert(m_socket);
	return setsockopt(m_socket,nOptLevel,nOptName,szOptVal,nOptLen);
}
int KSocket::GetOption(int nOptName,char* szOptVal,int& nOptLen,int nOptLevel){
	assert(m_socket);
	return getsockopt(m_socket,nOptLevel,nOptName,szOptVal,&nOptLen);
}

//
unsigned short KSocket::GetLocalPort(void){
	sockaddr_in addr;
	if(GetLocalAddr(&addr)==0)
		return addr.sin_port;
	return 0;
}
unsigned short KSocket::GetRemotePort(void){
	sockaddr_in addr;
	if(GetRemoteAddr(&addr)==0)
		return addr.sin_port;
	return 0;
}
int KSocket::GetLocalAddr(sockaddr_in* pAddr){
	int nAddrLen=sizeof(sockaddr_in);
	return getpeername(m_socket,(sockaddr*)pAddr,&nAddrLen);
}
int KSocket::GetRemoteAddr(sockaddr_in* pAddr){
	int nAddrLen=sizeof(sockaddr_in);
	return getsockname(m_socket,(sockaddr*)pAddr,&nAddrLen);
}

KClientCommand::~KClientCommand(){}
KServerCommand::~KServerCommand(){}

//KTCPAcceptor
KTCPAcceptor::KTCPAcceptor(unsigned short port,char* ip_or_name):m_nPort(port),m_szIPOrName(ip_or_name){}

bool KTCPAcceptor::Open(){
	if(m_socket.Bind(m_nPort,m_szIPOrName)==0)
		if(m_socket.Listen()==0)
			return true;
	return false;
}
KSocket KTCPAcceptor::Accept(){
	return m_socket.Accept();
}

//
KTCPConnector::KTCPConnector(unsigned short port,char* ip_or_name):m_nPort(port),m_szIPOrName(ip_or_name){}
bool KTCPConnector::Open(){
	memset(&m_addrRemote,0,sizeof(m_addrRemote));

	m_addrRemote.sin_family	=AF_INET;
	m_addrRemote.sin_port	=::htons(m_nPort);

	if(m_szIPOrName){
		hostent*	pHostEntry;
		ULONG		uAddr=::inet_addr(m_szIPOrName);

		if(INADDR_NONE==uAddr){
			if(pHostEntry=::gethostbyname(m_szIPOrName))
				uAddr=*((ULONG *)pHostEntry->h_addr_list[0]);
			else
				return false;
		}
		m_addrRemote.sin_addr.s_addr=uAddr;
	}
	else
		m_addrRemote.sin_addr.s_addr=INADDR_ANY;
		
	return true;
}
KSocket KTCPConnector::Connect(){
	return m_socket.Connect(&m_addrRemote);
}

