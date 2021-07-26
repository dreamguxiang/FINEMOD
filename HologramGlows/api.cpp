#include "pch.h"
#include "api.h"
#define AFX_14BEC153_17B9_47BE_845F_71A27BF26B59__INCLUDED_  
#include "llmoney.h"
#if _MSC_VER > 1000  
#pragma once  
#endif // _MSC_VER > 1000  

#include <iostream>  
#include <string>  
#include <windows.h> 
using namespace std;

string cpuname;
string diskdrivename;
string osname;
std::string string_To_UTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    ZeroMemory(pwBuf, nwLen * 2 + 2);
    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    ZeroMemory(pBuf, nLen + 1);
    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
    std::string retStr(pBuf);
    delete[]pwBuf;
    delete[]pBuf;
    pwBuf = NULL;
    pBuf = NULL;
    return retStr;
}

string& ClearHeadTailSpace(string& str)
{
    if (str.empty())
    {
        return str;
    }

    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;

}

string& ClearHeadTailn(string& str)
{
    if (str.empty())
    {
        return str;
    }

    str.erase(0, str.find_first_not_of("\n"));
    str.erase(str.find_last_not_of("\n") + 1);
    return str;

}

string getmoney(WPlayer&wp) {
	return to_string(Money::getMoney(offPlayer::getXUID(wp)));
}

string gettime()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", LocTime);
	return string(timestr);
}

string getversion() {
	auto MajorVersion = SymCall("?MajorVersion@SharedConstants@@3HB",int*);
	auto MinorVersion = SymCall("?MinorVersion@SharedConstants@@3HB",int*);
	auto PatchVersion = SymCall("?PatchVersion@SharedConstants@@3HB",int*);
	auto RevisionVersion = SymCall("?RevisionVersion@SharedConstants@@3HB",int*);
	std::string name = ".";
	auto first = std::to_string(*(int*)MajorVersion);
	first += name;
	first += std::to_string(*(int*)MinorVersion);
	first += name;
	first += std::to_string(*(int*)PatchVersion);
	first += name;
	first += std::to_string(*(int*)RevisionVersion);
	return first;
}

string getProtocolVersion() {
	auto v = SymCall("?NetworkProtocolVersion@SharedConstants@@3HB",int*);
	auto first = std::to_string(*(int*)v);
	return first;
}

string getplayerpermlevel(WPlayer&wp) {
	string permLevel;
	auto plevel = wp->getCommandPermissionLevel();
	switch (plevel) {
	case 1:
		permLevel = u8"§c§l管理员";
		break;
	case 0:
		permLevel = u8"§a§l玩家";
		break;
	default:
		permLevel = std::to_string(plevel);
	}
	return permLevel;
}