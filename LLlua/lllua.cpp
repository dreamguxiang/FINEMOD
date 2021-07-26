// bdxlua.cpp : 定义 DLL 的导出函数。
//
#include "pch.h"
#include "lllua.h"
#include "luaBindings.h"
#include <map>
extern std::map<string_view, std::pair<string_view, int>> cmds;
std::string Version = "210530";
using namespace std;
static vector<LModule> Modules;
LLLUA_API void registerLuaModule(LModule x) {
	Modules.push_back(x);
}

lua_State* _L;
bool _LIsInMain;
int _LEHIDX;

LLLUA_API  optional<long long> call_lua(const char* name, std::initializer_list<ldata_ref_t> const& arg) {
	LuaStackBalance B(_L);
	int EHIDX(getEHIDX());
	if (lua_getglobal(_L, name) == 0) {
		printf("[LUA] function %s not found\n", name);
		return {};
	}
	LuaFly fly{ _L };
	for (auto& i : arg) {
		if (i.isLong) {
			fly.push(i.asLL());
		}
		else {
			fly.push(i.asStr());
		}
	}
	if (!fly.pCall(name, arg.size(), 1, EHIDX)) {
		printf("[LUA] <%s> Error: %s\n", name, lua_tostring(_L, -1));
		return {};
	}
	if (lua_isinteger(_L, -1)) {
		long long x;
		fly.pop(x);
		return { {x} };
	}
	else {
		return {};
	}
}

static bool isReload = false;
bool dofile_lua(string const& name, bool HandleException = false) {
	std::ifstream ifs(name);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	int EHIDX = 0;
	if (HandleException) {
		lua_getglobal(_L, "EXCEPTION");
		EHIDX = lua_gettop(_L);
	}
	auto state = luaL_loadbufferx(_L, str.data(), str.size(), name.c_str(), nullptr);
	if (state != 0) {
		auto str = lua_tostring(_L, -1);
		printf("load lua error %s\n", str);
		if (!isReload)
			exit(1);
		return false;
	}
	else {
		auto rv = lua_pcall(_L, 0, 0, EHIDX);
		if (rv != 0) {
			auto str = lua_tostring(_L, -1);
			printf("call lua err %s\n", str);
			if (!isReload)
				exit(1);
			return false;
		}
	}
	lua_settop(_L, EHIDX == 0 ? 0 : (EHIDX - 1));
	return true;
}
#include<filesystem>
#include"framework.h"
string wstr2str(std::wstring const& str) {
	string res;
	int len = WideCharToMultiByte(GetACP(), 0, str.c_str(), int(str.size()), nullptr, 0, nullptr, nullptr);
	res.append(len, 0);
	WideCharToMultiByte(GetACP(), 0, str.c_str(), int(str.size()), res.data(), int(res.size()), nullptr, nullptr);
	return res;
}
bool loadlua() {
	if (_L) {
		for (auto i : Modules) {
			i.defunct(_L);
		}
		lua_close(_L);
	}
	_L = luaL_newstate();
	luaL_openlibs(_L);
	for (auto i : Modules) {
		i.init(_L);
	}
	using namespace std::filesystem;
	_LIsInMain = false;
	dofile_lua("plugins/LiteLuaLoader/lua/init.lua");
	lua_getglobal(_L, "EXCEPTION");
	_LEHIDX = luaL_ref(_L, LUA_REGISTRYINDEX);
	_LIsInMain = true;
	{
		directory_iterator ent("plugins/LiteLuaLoader/lua");
		for (auto& i : ent) {
			if (i.is_regular_file() && i.path().extension() == ".lua" && i.path().filename() != "init.lua") {
				dofile_lua(wstr2str(i.path()), true);
			}
		}
	}
	lua_settop(_L, 0);
	lua_getglobal(_L, "EXCEPTION");
	return true;
}
bool oncmd_lua(CommandOrigin const& ori, CommandOutput& outp, string& fn) {
	auto caller = ori.getEntity();
	call_luaex(("u_" + fn).c_str(), (uintptr_t)caller);
	return true;
}
bool oncmd_reloadlua(CommandOrigin const& ori, CommandOutput& outp) {
	return loadlua();
}
void initItemM();
void initNBTM();
void initBlockM();
bool oncmd_gui(CommandOrigin const& ori, CommandOutput& outp, string& v);
void entry() {
	Event::addEventListener([](PostInitEV ev) {
		loadlua();
		isReload = true;
		});
	cout << "[LiteLuaLoader] LiteLuaLoader Loaded, By LiteLDev, Build Date [" << __TIMESTAMP__ << u8"]" << endl;
	cout << "[LiteLuaLoader] Version " << Version << endl;
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("lcall", "call lua fn", 0);
		CmdOverload(lcall, oncmd_lua, "func name");
		MakeCommand("lreload", "reload lua", 1);
		CmdOverload(lreload, oncmd_reloadlua);
		MakeCommand("gui", "show gui", 0);
		CmdOverload(gui, oncmd_gui, "path");
		for (auto i : cmds) {
			MakeCommand(i.first.data(), i.second.first.data(), i.second.second);
		}
		});
	//Player::Player(Level &,PacketSender &,GameType,NetworkIdentifier const &,uchar,mce::UUID,std::basic_string<char,std::char_traits<char>,std::allocator<char>> const &,std::unique_ptr<Certificate,std::default_delete<Certificate>>,std::basic_string<char,std::char_traits<char>,std::allocator<char>> const &,std::basic_string<char,std::char_traits<char>,std::allocator<char>> const &)	.text	00000001408CFE30	0000177B	00000788	00000058	R	.	.	.	B	T	.
#define LuaMod(x) registerLuaModule(x())
	initItemM();
	initNBTM();
	initBlockM();
	LuaMod(luaThreading_module);
	LuaMod(luaEvent_module);
	LuaMod(lualog_module);
	LuaMod(luafs_module);
	LuaMod(luaDB_module);
	LuaMod(luaBase_module);
	LuaMod(luagui_module);
	LuaMod(luaSch_module);
}
