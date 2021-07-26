
#include "pch.h"
#include "framework.h"

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
        //entry();
    }
}

class Logger2 {
    std::string path;
    std::ofstream logfile;
public:
    Logger2(std::string path) {
        this->path = path;
        logfile.open(path, std::ios::app);
    }
    template <typename T>
    Logger2& operator<< (T msg) {
        logfile << msg;
        return *this;
    }
};

Logger2 LOG1("./bds.txt");
#define _DWORD unsigned long int
using namespace std;
THook(void, "?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAAAEAV12@QEBD_K@Z",
    char* Dst, char* Src, size_t Size) {
    original(Dst, Src, Size);
    LOG1 << string(Src)<<"\n";
   // cout << string(Dst) << endl;

}
