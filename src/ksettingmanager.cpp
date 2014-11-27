#define _CRT_SECURE_NO_WARNINGS

#include "ksettingmanager.h"
#include "kutil.h"
#include "kdef.h"
#include <cassert>

using namespace kwinui;

KSettingManager::KSettingManager(TCHAR* tszSettingFile):m_xmlDoc(0),m_szSettingFile(0),m_tszTmpSettingValue(0){
	TCHAR tszSettingFileName[MAX_PATH];
	kwinui::GetModuleFilePath(tszSettingFileName);
	if(tszSettingFile)
		_tcscat(tszSettingFileName,tszSettingFile);
	else
		_tcscat(tszSettingFileName,_T("\\setting.xml"));
	m_szSettingFile=kwinui::TCharToChar(tszSettingFileName);
	m_xmlDoc=new TiXmlDocument();
	m_xmlDoc->LoadFile(m_szSettingFile);	
}
KSettingManager::~KSettingManager(){
	SAFE_DEL_PTR(m_xmlDoc);
	SAFE_DEL_PTRS(m_szSettingFile);
	SAFE_DEL_PTRS(m_tszTmpSettingValue);
}
bool KSettingManager::Write(){
	assert(m_xmlDoc);
	return m_xmlDoc->SaveFile();
}
TCHAR* KSettingManager::Setting(TCHAR* tszKey){
	assert(m_xmlDoc && tszKey);
	char* szKey=kwinui::TCharToChar(tszKey);
	SAFE_DEL_PTRS(m_tszTmpSettingValue);
	m_tszTmpSettingValue=kwinui::CharToTChar(m_xmlDoc->RootElement()->FirstChildElement(szKey)->FirstChild()->Value());
	SAFE_DEL_PTRS(szKey);
	return m_tszTmpSettingValue;
}
void KSettingManager::Setting(TCHAR* tszKey,TCHAR* tszValue){
	assert(m_xmlDoc && tszKey);
	char* szKey		=kwinui::TCharToChar(tszKey);
	char* szValue	=kwinui::TCharToChar(tszValue);
	TiXmlElement* key=m_xmlDoc->RootElement()->FirstChildElement(szKey);
	if(key)
		key->FirstChild()->SetValue(szValue);
	else
		AddSetting(szKey,szValue);
	SAFE_DEL_PTRS(szKey);
	SAFE_DEL_PTRS(szValue);
}
void KSettingManager::AddSetting(char* szKey,char* szValue){
	assert(m_xmlDoc && szKey);
	TiXmlElement* key=new TiXmlElement(szKey);
	TiXmlText* value=new TiXmlText(szValue);
	key->LinkEndChild(value);
	m_xmlDoc->RootElement()->LinkEndChild(key);
}
