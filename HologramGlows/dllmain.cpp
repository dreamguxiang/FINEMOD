// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"

void  entry_npc();
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
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
        entry_npc();
    }
}

LIAPI string GetDataPath(const string& myname) {
    using namespace std::filesystem;
    create_directory("plugins\\HologramGlows");
    create_directory("plugins\\HologramGlows\\data");
    return "plugins\\HologramGlows\\data\\" + myname;
}