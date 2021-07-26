#include"pch.h"
#include"luaBindings.h"
#include<filesystem>

static int lb_fs_del(lua_State* L) {
	try {
		LuaFly fl(L);
		string fname;
		fl.pop(fname);
		lua_pushinteger(L,std::filesystem::remove_all(fname));
		return 1;
	}CATCH()
}
static int lb_fs_mkdir(lua_State* L) {
	try {
		LuaFly fl(L);
		string fname;
		fl.pop(fname);
		std::filesystem::create_directories(fname);
		return 0;
	}CATCH()
}
static int lb_fs_copy(lua_State* L) {
	try {
		LuaFly fl(L);
		string from,to;
		fl.pops(to,from);
		std::filesystem::copy(from, to);
		return 0;
	}CATCH()
}
static int lb_fs_rename(lua_State* L) {
	try {
		LuaFly fl(L);
		string from, to;
		fl.pops(to, from);
		std::filesystem::rename(from, to);
		return 0;
	}CATCH()
}
static int lb_fs_exists(lua_State* L) {
	try {
		LuaFly fl(L);
		string fn;
		fl.pop(fn);
		lua_pushboolean(L,std::filesystem::exists(fn));
		return 1;
	}CATCH()
}
static const luaL_Reg R[] =
{
	{ "del",	lb_fs_del	},
	{ "copy",	lb_fs_copy	},
	{"rename",lb_fs_rename},
	{"mkdir",lb_fs_mkdir},
	{"exists",lb_fs_exists},
	{ NULL,		NULL	}
};
static void lb_fs_entry(lua_State* L) {
	lua_newtable(L);
	luaL_setfuncs(L, R, 0);
	lua_setglobal(L, "fs");
}
LModule luafs_module() {
	return { lb_fs_entry };
}