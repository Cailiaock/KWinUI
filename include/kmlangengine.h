/*
 *	klangengine.h---MultiLanguage Engine
 *	proguru (C) 2008
 *	03/10/2008
 *
 */
#ifndef __KLANGENGINE_H__
#define __KLANGENGINE_H__

#include "tinyxml.h"
#include "KDListT.h"
#include "kdef.h"
#include "kthunk.h"

namespace kwinui{

struct LangItem{
	int 	nID;
	TCHAR*	tszString;
	LangItem(){
		nID=0;
		tszString=0;
	}
	LangItem(int id,TCHAR* tszStr){
		nID=id;
		tszString=tszStr;
	}
};

class KLangEngine{
public:
	KLangEngine(TCHAR* tszLangFile=0);
	~KLangEngine();

	void SetCaptionDenscent(HWND hWndParent,int nIDTopLevel);
	void SetCaptionChild(HWND hWndParent,int nIDSParent);
	TCHAR* GetString(int nID);
	void SetMenuText(HMENU hMnuMain);
	void SetMainMenuText(HWND hWnd,int nStartID);
private:
	typedef LangItem* 			PLangItem;
	typedef KDListT<PLangItem> 	listLang;
	listLang		m_listLang; //store items of specific language
	KGeneralThunk	m_thunk;
	
	bool ChangeLanguage(TCHAR* tszNewLangFile);
	INT_PTR GetThreadProcPtr(){	
		typedef BOOL (CALLBACK KLangEngine::*proc_t)(HWND,LPARAM);
		union{
			proc_t	 	proc;
			INT_PTR		dwProcAddr;			
		}u;
		u.proc=&KLangEngine::EnumChildWindowProc;	
		return u.dwProcAddr;
	}
	BOOL CALLBACK EnumChildWindowProc(HWND hWndChild,LPARAM lParam);	
};
}//namespace kwinui
#endif //__KLANGENGINE_H__
