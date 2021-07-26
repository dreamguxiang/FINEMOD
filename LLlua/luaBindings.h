#pragma once
#include"LLlua.h"


bool CallEvent(const char* name, std::initializer_list<ldata_ref_t> const& arg);

LModule luaThreading_module();
LModule luaEvent_module();
LModule lualog_module();
LModule luafs_module();
LModule luaDB_module();
LModule luaBase_module();
LModule luagui_module();
LModule luaSch_module();


