// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include<lbpch.h>
#include "framework.h"
#include<lua.hpp>
#include<fstream>
#include<streambuf>
#include<api\regCommandHelper.h>
#include<api/basicEvent.h>
#include<api/types/types.h>
#include<api/gui/gui.h>
#include"framework.h"
#include<stl/KVDB.h>
#include<mc/Level.h>
#include<mc/Actor.h>
#include<mc/OffsetHelper.h>
#include<mc\Player.h>
#include<api\myPacket.h>
#include<LuaBridge/LuaBridge.h>
#define addApi(x) int x(lua_State* L)
#define addApiEx(a,b) int a(lua_State* L) {\
    try\
         b\
    CATCH()\
}
using std::ifstream, std::ofstream, std::stringstream;
extern std::string Version;
#endif //PCH_H
