/*
 * multilanguage dialog base
 * proguru,2009
*/

#ifndef __KMLANGDLG_H__
#define __KMLANGDLG_H__

#include "kwin.h"
#include "kmlangengine.h"
#include "ksettingmanager.h"

namespace kwinui;

//Multilingual Dialog Base
template<typename T,typename TBase=KWindow>
class KMLangDlgBase : public KDialogBase<T,TBase>{
public:
	typedef KDialogBase<T,TBase> __base;
	K_BEGIN_MSG_MAP
		K_MSG_HANDLER(WM_INITDIALOG,OnLanguageChange)
		K_MSG_HANDLER(WM_EX_LANGCHANGE,OnLanguageChange)
	K_END_MSG_MAP(__base)

	LRESULT OnLanguageChange(UINT uMsg,WPARAM wParam,LPARAM lParam,bool& bHandled){
		KLangEngine* pLangEngine=new KLangEngine();
		pLangEngine->SetCaptionChild(m_hWnd,T::IDD);
		SAFE_DEL_PTR(pLangEngine);
		bHandled=false;
		return TRUE;
	}
};

#define LANG_SUBMENU_ID_START	50000
#define MAIN_MENU_ITEM_ID_START 30000
template<typename T,typename TBase=KWindow>
class KMLangMainDlgBase : public KMLangDlgBase<T,TBase>{
public:
	typedef KMLangDlgBase<T,TBase> __base;
	KMLangMainDlgBase():m_hMnuPopupLang(0),m_ptszLangFileName(0),m_nMenuItemCount(0){
	}
	~KMLangMainDlgBase(){
		for(int i=0;i<m_nMenuItemCount;i++){
			SAFE_DEL_PTRS(m_ptszLangFileName[i]);
		}
		SAFE_DEL_PTRS(m_ptszLangFileName);
	}

	K_BEGIN_MSG_MAP
		K_MSG_HANDLER(WM_INITDIALOG,OnInitDialog)
		K_MSG_HANDLER(WM_EX_LANGCHANGE,OnLangChange)
		K_COMMAND_ID_RANGE_HANDLER(LANG_SUBMENU_ID_START,m_nMenuItemCount,OnLangMenuItem)
	K_END_MSG_MAP(__base)

	LRESULT OnInitDialog(UINT uMsg,WPARAM wParam,LPARAM lParam,bool& bHandled){
		SetMainMenuText();
		InsertLangSubMenu();
		bHandled=false;
		return TRUE;
	}
	LRESULT OnLangChange(UINT uMsg,WPARAM wParam,LPARAM lParam,bool& bHandled){
		SetMainMenuText();
		bHandled=false;
		return 0;
	}
	LRESULT OnLangMenuItem(WORD wID,WORD wNotifyCode,HWND hWndCtrl,bool& bHandled){
		int nLangID=wID-LANG_SUBMENU_ID_START;

		KSettingManager* pSettingMgr=new KSettingManager();
		pSettingMgr->Setting(_T("Language"),m_ptszLangFileName[nLangID]);
		pSettingMgr->Write();
		SAFE_DEL_PTR(pSettingMgr);

		::CheckMenuRadioItem(m_hMnuPopupLang,0,m_nMenuItemCount,nLangID,MF_BYPOSITION);
		SendMessage(WM_EX_LANGCHANGE);
		return 0;
	}
private:
	HMENU CreateLangSubMenu(){
		m_hMnuPopupLang=::CreatePopupMenu();

		TCHAR tszLangPath[MAX_PATH]={_T('\0')};
		kwin::GetModuleFilePath(tszLangPath);

		KSettingManager* pSettingMgr=new KSettingManager();
		::_tcscat(tszLangPath,_T("\\"));
		::_tcscat(tszLangPath,pSettingMgr->Setting(_T("LangPath")));
		::_tcscat(tszLangPath,_T("\\*"));

		WIN32_FIND_DATA	wfd;
		memset(&wfd,'\0',sizeof(WIN32_FIND_DATA));
		
		bool bFindFinish=false;
		int nLenTmp=0;
		MENUITEMINFO mii;

		HANDLE hFindFile=::FindFirstFile(tszLangPath,&wfd);
		if(hFindFile==INVALID_HANDLE_VALUE)
			bFindFinish=true;
		while(!bFindFinish){
			if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))		//not directory
				m_nMenuItemCount++;			
			bFindFinish=0==::FindNextFile(hFindFile,&wfd);
		}
		::FindClose(hFindFile);

		m_ptszLangFileName=new TCHAR*[m_nMenuItemCount];

		bFindFinish=false;
		hFindFile=::FindFirstFile(tszLangPath,&wfd);
		if(hFindFile==INVALID_HANDLE_VALUE)
			bFindFinish=true;
		int nCount=0;
		while(!bFindFinish){
			if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){			//not directory
				TCHAR* tszTmp=new TCHAR[::_tcslen(wfd.cFileName)+1];	
				::_tcscpy(tszTmp,wfd.cFileName);
				m_ptszLangFileName[nCount]=tszTmp;

				nLenTmp=::_tcslen(tszTmp)-4;	
				tszTmp[nLenTmp]=_T('\0');

				memset(&mii,0,sizeof(MENUITEMINFO));
				mii.cbSize		=sizeof(MENUITEMINFO);
				mii.fMask		=MIIM_TYPE|MIIM_ID;
				mii.fType		=MFT_STRING|MFT_RADIOCHECK;
				mii.wID			=LANG_SUBMENU_ID_START+nCount++;
				mii.dwTypeData	=tszTmp;

				::InsertMenuItem(m_hMnuPopupLang,-1,TRUE,&mii);
				tszTmp[nLenTmp]=_T('.');
			}
			bFindFinish=0==::FindNextFile(hFindFile,&wfd);
		}
		::FindClose(hFindFile);

		//
		TCHAR* tszLang=pSettingMgr->Setting(_T("Language"));
		for(int i=0;i<m_nMenuItemCount;i++){
			if(!::_tcscmp(tszLang,m_ptszLangFileName[i]))
			::CheckMenuRadioItem(m_hMnuPopupLang,0,m_nMenuItemCount,i,MF_BYPOSITION);
		}
		SAFE_DEL_PTR(pSettingMgr);
		return m_hMnuPopupLang;
	}
	void InsertLangSubMenu(){
		HMENU hMnuMain=::GetMenu(m_hWnd);
		if(!hMnuMain)
			return;

		HMENU hMnuInsert=::GetSubMenu(hMnuMain,1);
		int nDummy=0;
		//SearchMenuItem(::GetMenu(m_hWnd),_T("&Options"),hMnuInsert,nDummy);

		MENUITEMINFO mii;
		memset(&mii,0,sizeof(MENUITEMINFO));
		mii.cbSize		=sizeof(MENUITEMINFO);
		mii.fMask		=MIIM_STRING|MIIM_SUBMENU;
		mii.fType		=MFT_STRING;
		mii.dwTypeData	=_T("&Languages");
		mii.hSubMenu	=CreateLangSubMenu();

		::InsertMenuItem(hMnuInsert,-1,TRUE,&mii);
	}
	void SearchMenuItem(HMENU hMenu,TCHAR* tszMenuItemString,HMENU& hDstMenu,int& nItemPos){
		MENUITEMINFO mii;

		int nMenuItemCnt=::GetMenuItemCount(hMenu);
		int nMenuItemID=0;
		HMENU hSubMenu=0;
		for(int i=0; i<nMenuItemCnt; i++){
			memset(&mii,0,sizeof(MENUITEMINFO));
			mii.cbSize	=sizeof(MENUITEMINFO);
			mii.fMask	=MIIM_FTYPE|MIIM_STRING;
			mii.fType	=MFT_STRING;

			::GetMenuItemInfo(hMenu,i,TRUE,&mii);
			TCHAR* tszString=new TCHAR[++mii.cch];
			mii.dwTypeData	=tszString;
			::GetMenuItemInfo(hMenu,i,TRUE,&mii);
			if(!::_tcscmp(tszMenuItemString,tszString)){
				hDstMenu=hMenu;
				nItemPos=i;
			}
			
			hSubMenu=::GetSubMenu(hMenu,i);
			if(hSubMenu)
				SearchMenuItem(hSubMenu,tszMenuItemString,hDstMenu,nItemPos);
		}
	}
	void SetMainMenuText(){
		HMENU hMnuMain=::GetMenu(m_hWnd);
		if(hMnuMain){
			KLangEngine* pLang=new KLangEngine();
			pLang->SetMainMenuText(m_hWnd,MAIN_MENU_ITEM_ID_START);
			pLang->SetMenuText(hMnuMain);
		}
	}
private:
	int		m_nMenuItemCount;
	HMENU	m_hMnuPopupLang;
	TCHAR**	m_ptszLangFileName;//
};
#endif //__KMLANGDLG_H__
