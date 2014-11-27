/*
 * ksettingmanager.h --- app setting manager
 * 
 * 03/15/2008
 * proguru
 *
 */
#ifndef __KSETTINGMANAGER_H__
#define __KSETTINGMANAGER_H__

#include "tinyxml.h"
#include "tchar.h"

namespace kwinui{

class KSettingManager{
public:
	KSettingManager(TCHAR* tszSettingFile=0);
	~KSettingManager();

	bool Write();
	TCHAR* Setting(TCHAR* tszKey);
	void Setting(TCHAR* tszKey,TCHAR* tszValue);
private:
	void AddSetting(char* szKey,char* szValue);
private:
	TiXmlDocument*	m_xmlDoc;
	char*			m_szSettingFile;
	TCHAR*			m_tszTmpSettingValue;
};
}//namespace ksettingmgr
#endif //__SETTINGMANAGER_H__
