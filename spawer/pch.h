// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H
// 添加要在此处预编译的标头
#include<lbpch.h>
#include<stl\Bstream.h>
#include<stl\viewhelper.h>
#include<stl\views.h>
#include<stl\KVDB.h>
#include<stl\langPack.h>
#include<stl\Logger.h>
#include<stl/LRUMap.h>
#include<stl/static_queue.h>
#include<random>
#include<iostream>
#include<LOADER/Loader.h>
#include<api\commands.h>
#include<api\xuidreg\xuidreg.h>
#include<api\refl\playerMap.h>
#include<stl\langPack.h>
#include<mc\Level.h>
#include<mc/Player.h>
#include<mc/Item.h>
#include<api/Basic_Event.h>
#include<mc/Block.h>
#include<mc/BlockSource.h>
#include<mc/OffsetHelper.h>
#endif //PCH_H