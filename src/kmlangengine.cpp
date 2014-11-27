/*
 * klangengine.h--MultiLanguage Engine
 * proguru (C) 2008
 * 03/10/2008 (d/m/y)
 *
 */
#define _CRT_SECURE_NO_WARNINGS

#include "kmlangengine.h"
#include "kutil.h"
#include "ksettingmanager.h"
#include <cassert>

using namespace kwinui;

KLangEngine::KLangEngine(TCHAR* tszLangFile){
	m_thunk.Init((INT_PTR)this,GetThreadProcPtr());
	ChangeLanguage(tszLangFile);	
}

KLangEngine::~KLangEngine(){
	listLang::iterator iter;
	for(iter=m_listLang.begin();iter!=m_listLang.end();iter++){
		delete (*iter)->tszString;
		delete (*iter);
	}
}
bool KLangEngine::ChangeLanguage(TCHAR* tszNewLangFile){
	TCHAR tszLangFile[MAX_PATH]={_T('\0')};
	kwinui::GetModuleFilePath(tszLangFile);

	KSettingManager* pSettingMgr=new KSettingManager();
	::_tcscat(tszLangFile,_T("\\"));	
	::_tcscat(tszLangFile,pSettingMgr->Setting(_T("LangPath")));	
	::_tcscat(tszLangFile,_T("\\"));	

	if(tszNewLangFile)
		_tcscat(tszLangFile,tszNewLangFile);
	else
		::_tcscat(tszLangFile,pSettingMgr->Setting(_T("Language")));
	
	SAFE_DEL_PTR(pSettingMgr);

	char* szLangFile=kwinui::TCharToChar(tszLangFile);

	TiXmlDocument* pXmlDocLang=new TiXmlDocument();
	pXmlDocLang->SetCondenseWhiteSpace(false);
	if(pXmlDocLang->LoadFile(szLangFile)){
		TiXmlElement* pEmtLang=0;
		TiXmlNode *pEmtChild=0;

		if(pEmtLang=pXmlDocLang->RootElement()){
			while(pEmtChild=pEmtLang->IterateChildren(pEmtChild)){
				PLangItem pli=new LangItem(::atoi(pEmtChild->Value()+3),
					kwinui::CharToTChar(const_cast<char*>(pEmtChild->FirstChild()->Value()),true));
				m_listLang.push_back(pli);
			}
			delete pXmlDocLang;
			delete szLangFile;
			return true;
		}
	}
	return false;
}
void KLangEngine::SetCaptionDenscent(HWND hWndParent,int nIDTopLevel){
	::SetWindowText(hWndParent,GetString(nIDTopLevel));
	::EnumChildWindows(hWndParent,(BOOL (CALLBACK*)(HWND,LPARAM))m_thunk.GetThunkedCodePtr(),0);
}
BOOL CALLBACK KLangEngine::EnumChildWindowProc(HWND hWndChild,LPARAM lParam){
	TCHAR* tszString=GetString(::GetDlgCtrlID(hWndChild));
	if(tszString)
		::SetWindowText(hWndChild,GetString(::GetDlgCtrlID(hWndChild)));
	return TRUE;
}
void KLangEngine::SetCaptionChild(HWND hWndParent,int nIDSParent){
	::SetWindowText(hWndParent,GetString(nIDSParent));

	HWND hWndChild=::GetWindow(hWndParent,GW_CHILD);
	while(hWndChild){
		TCHAR* tszString=GetString(::GetDlgCtrlID(hWndChild));
		if(tszString)
			::SetWindowText(hWndChild,tszString);

		hWndChild=::GetNextWindow(hWndChild,GW_HWNDNEXT);
	}
}

TCHAR* KLangEngine::GetString(int id){
	listLang::iterator iter;
	for(iter=m_listLang.begin();iter!=m_listLang.end();iter++)
		if((*iter)->nID==id)
			return (*iter)->tszString;
	
	return 0;
}

void KLangEngine::SetMainMenuText(HWND hWnd,int nStartID){
	assert(hWnd);
	HMENU hMainMenu=::GetMenu(hWnd);
	MENUITEMINFO mii;
	memset(&mii,0,sizeof(MENUITEMINFO));
	mii.cbSize	=sizeof(MENUITEMINFO);
	mii.fMask	=MIIM_STRING;

	int nMenuItemCnt=::GetMenuItemCount(hMainMenu);
	for(int i=0; i<nMenuItemCnt; i++){
		if(mii.dwTypeData=GetString(nStartID+i))
			::SetMenuItemInfo(hMainMenu,i,TRUE,&mii);
	}
	::DrawMenuBar(hWnd);
}

void KLangEngine::SetMenuText(HMENU hMenu){
	assert(hMenu);

	MENUITEMINFO mii;
	memset(&mii,0,sizeof(MENUITEMINFO));
	mii.cbSize	=sizeof(MENUITEMINFO);
	mii.fMask	=MIIM_STRING;

	int nMenuItemCnt=::GetMenuItemCount(hMenu);
	int nMenuItemID=0;
	HMENU hSubMenu=0;
	for(int i=0; i<nMenuItemCnt; i++){
		nMenuItemID		=::GetMenuItemID(hMenu,i);
		if(mii.dwTypeData=GetString(nMenuItemID))
			::SetMenuItemInfo(hMenu,i,TRUE,&mii);

		hSubMenu=::GetSubMenu(hMenu,i);
		if(hSubMenu)
			SetMenuText(hSubMenu);
	}
}
