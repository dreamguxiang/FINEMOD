// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
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



THook(void, "?_Throw_bad_optional_access@std@@YAXXZ") {
    throw 1;
}

THook(void, "?getEntities@LevelChunk@@QEBAXV?$span@V?$not_null@PEBVActor@@@gsl@@$0?0@gsl@@AEBVAABB@@AEAV?$vector@PEAVActor@@V?$allocator@PEAVActor@@@std@@@std@@_N@Z",
    __int64 a1, char* a2, const struct AABB* a3, __int64 a4, char a5) {
    try {
        original(a1, a2, a3, a4, a5);
    }
    catch (...) {
        return;
    }
}