// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"

//void entry();

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        cout << "[AntiToolbox]  version 1.17.10.4 " << endl;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        //entry();
    }
}

#include <mc/mass.h>
#include <ezmc/Core/ConnectionRequest.h>
#include <regex>
vector<string> res_token;

vector<string> isWord(string str) {
    vector<string> words;
    int t = 0, i = 0, w = 0;
    string word;
    while (i < str.length()) {
        while (str[i] == ' ')
            i++;
        t = i;
        w = 0;
        while (str[i] != ' ' && i < str.length())
        {
            i++;
            w++;
        }
        word = str.substr(t, w);
        words.push_back(word);
    }
    return words;
}

bool isssad(string a1) {
    int t = 0, i = 0, w = 0;
    while (i < a1.length()) {
        if (!islower(a1[i])) {
            i++;
        }
        else {
            return 1;
        }
    }
    return 0;
}
string getversion() {
    auto MajorVersion = SymCall("?MajorVersion@SharedConstants@@3HB", int*);
    auto MinorVersion = SymCall("?MinorVersion@SharedConstants@@3HB", int*);
    auto PatchVersion = SymCall("?PatchVersion@SharedConstants@@3HB", int*);
    auto RevisionVersion = SymCall("?RevisionVersion@SharedConstants@@3HB", int*);
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
THook(void, "?startServerThread@ServerInstance@@QEAAXXZ", void* a) {
    original(a);
    auto vm = getversion();
    if (vm != "1.17.10.4"){
        cout<<u8"当前插件版本为1.17.10.4，服务端版本为"<< vm <<",等待版本适配!" << endl;
        Sleep(5000);
        exit(1);
    }
}
THook(void, "?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@AEBVConnectionRequest@@AEAVServerPlayer@@@Z",
     ServerNetworkHandler* a1, class NetworkIdentifier* a2,  ConnectionRequest* a3,class ServerPlayer* a4) {
    auto map1 = a3->rawToken->dataInfo.value_.map_;
    for (auto iter = map1->begin(); iter != map1->end(); ++iter) {
        string s(iter->first.c_str());
        if (s.find("GameVersion") != s.npos) {
            auto gamev = iter->second.value_.string_;
        }
    }
    string mstr2 = "";
    string DeviceModel = "";
    auto out = SymCall("?getDeviceOS@ConnectionRequest@@QEBA?AW4BuildPlatform@@XZ", int, ConnectionRequest*, string*)(a3, &mstr2);
    if (out == 1) {
        for (auto iter = map1->begin(); iter != map1->end(); ++iter) {
            string s(iter->first.c_str());
            if (s.find("DeviceModel") != s.npos) {
                DeviceModel = iter->second.value_.string_;
                std::vector<std::string> str_list;
                auto a = isWord(DeviceModel);
                if (isssad(a[0])) {
                    a1->disconnectClient(*a2, u8"§6[AntiToolbox]§4请不要使用ToolBox进入游戏！！", 0);
                }
            }
        }
    }
    return original(a1, a2, a3, a4);
}