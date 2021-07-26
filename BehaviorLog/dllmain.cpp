// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <iostream>
#include <windows.h>
void BLog_entery();
extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        BLog_entery();
    }
}

