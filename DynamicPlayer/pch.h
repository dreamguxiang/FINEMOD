// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H
using namespace std;
// 添加要在此处预编译的标头
#include <iostream>
#include <loader/Loader.h>
#include<streambuf>
#include <map>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include <fstream>
#include <sstream>
#include <api/LiteLoaderApi.h>
#include <api/types/types.h>
#include <ctime>
#include <chrono>
#include<unordered_set>
#include<api/myPacket.h>
#include<stl/Bstream.h>
#include<stl/varint.h>
#include<lbpch.h>
#include<loader\hash.h>
#include<api\regCommandHelper.h>
#include<api/basicEvent.h>
#include<stl\Bstream.h>
#include<stl\langPack.h>
//#include<stl\LRUMap.h>
#include<ctime>
#include<api\xuidreg\xuidreg.h>
#include<stl\viewhelper.h>
#include<rapidjson/error/en.h>
#include<api\gui\gui.h>
#include<mc/OffsetHelper.h>
using std::shared_ptr, std::pair;
using std::make_shared;
#endif //PCH_H

