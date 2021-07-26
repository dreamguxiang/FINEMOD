// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "LLlua.h"

void entry(); //声明另一个cpp中定义的入口函数
extern "C" {
    _declspec(dllexport) void onPostInit() {  //Loader在装载完所有插件dll时会调用这个函数，为了安全请在这里进行初始化
        std::ios::sync_with_stdio(false); //为logger的io优化
        entry(); //插件初始化函数
    }
}

LIAPI string GetDataPath(const string& myname) {
    using namespace std::filesystem;
    create_directory("plugins\\LiteLuaLoader\\data");
    return "plugins\\LiteLuaLoader\\data\\" + myname;
}