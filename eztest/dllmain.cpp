// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include <JsonLoader.h>
#include <iostream>
#include "test.h"
void loadCfg();

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        loadCfg();
        std::cout << "[FallingDespawn] version 1.17.10.4 " << std::endl;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int FallingDespawn;
void loadCfg() {
    try {
        ConfigJReader jr("plugins/FallingDespawn/FallingDespawn.json");
        jr.bind("FallingDespawn", FallingDespawn,6000);
    }
    catch (string e) {
        printf("[FallingDespawn] json error %s\n", e.c_str());
        throw 0;
    }
}

class ItemActor;
HOOK(onItemActor, void, "?normalTick@ItemActor@@UEAAXXZ", ItemActor* a1) {
    if (FallingDespawn < 0) {
        std::cout << "[FallingDespawn]参数小于0,无法生效！\n";
        return original(a1);
    }

    dAccess<int, 0x7c4>(a1) = FallingDespawn;
    return original(a1);
}