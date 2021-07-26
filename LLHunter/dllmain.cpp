
#include "pch.h"
#include "framework.h"
#include <lbpch.h>
void entry();

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
        entry();
    }
}

LIAPI string GetDataPath(const string& myname) {
    using namespace std::filesystem;
    create_directory("plugins\\LiteLoader");
    return "plugins\\LiteLoader\\" + myname;
}