#include"pch.h"
#include"luaBindings.h"
#include<api\myPacket.h>
#include<stl\varint.h>
#include <mc/Item.h>
#include <map>
#include <api\xuidreg\xuidreg.h>
Minecraft* mc;
std::map<string_view, std::pair<string_view, int>> cmds;


bool inline isplayer(void* pl) {
	vector<Player*> pls = liteloader::getAllPlayers();
	return std::find(pls.begin(), pls.end(), pl) != pls.end();
}

int lb_sendText(lua_State* L) {
	try {
		//name text [type]
		auto n = lua_gettop(L);
		if (n < 3) {
			luaL_error(L, "sendText: need at least 2 args");
			return 0;
		}
		TextType tp{ RAW };
		if (n == 4) {
			tp = (TextType)lua_tointeger(L, 4);
		}
		auto pl = (Player*)(uintptr_t)lua_tointeger(L, 2);
		auto text = lua_tostring(L, 3);
		WPlayer(*pl).sendText(text, tp);
	}CATCH()
		return 0;
}

int lb_runcmd(lua_State* L) {
	auto n = lua_gettop(L);
	if (n < 1) {
		luaL_error(L, "runcmd(cmd)");
		return 0;
	}
	size_t s1;
	auto _n = lua_tolstring(L, 1, &s1);
	string_view cmd(_n, s1);
	bool rv;
	{
		LuaCtxSwapper S(L);
		rv = liteloader::runcmd(string{ cmd });
	}
	lua_pop(L, n);
	lua_pushboolean(L, rv);
	return 1;
}

int lb_runcmdex(lua_State* L) {
	auto n = lua_gettop(L);
	if (n < 1) {
		luaL_error(L, "runcmdex(cmd)");
		return 0;
	}
	size_t s1;
	auto _n = lua_tolstring(L, 1, &s1);
	string_view cmd(_n, s1);
	std::pair<bool, string> rvres;
	{
		LuaCtxSwapper S(L);
		rvres = liteloader::runcmdEx(string{ cmd });
	}
	lua_pop(L, n);
	lua_pushboolean(L, rvres.first);
	lua_pushlstring(L, rvres.second.data(), rvres.second.size());
	return 2;
}

int lb_runcmdAs(lua_State* L) {
	auto n = lua_gettop(L);
	if (n < 2) {
		luaL_error(L, "runcmdAs(pl,cmd)");
		return 0;
	}
	size_t s1;
	auto _n = lua_tolstring(L, 3, &s1);
	string_view cmd(_n, s1);
	auto pl = (void*)(uintptr_t)lua_tointeger(L, 2);
	if (isplayer(pl)) {
		optional<WPlayer> ply = WPlayer(*(ServerPlayer*)pl);
		if (ply.Set()) {
			bool rv;
			{
				LuaCtxSwapper S(L);
				rv = ply.val().runcmd(string{ cmd });
			}
			lua_pushboolean(L, rv);
		}
		else {
			lua_pushboolean(L, false);
		}
		return 1;
	}
}
namespace onlinepl {
	static string onlineLISTV;
	static void regenLISTV() {
		auto ps = liteloader::getAllPlayers();
		onlineLISTV.clear();
		onlineLISTV = '[';
		for (auto& i : ps) {
			auto pln = offPlayer::getRealName(i);
			onlineLISTV.push_back('"');
			onlineLISTV.append(pln);
			onlineLISTV.append("\",");
		}
		if (onlineLISTV.back() == ',')
			onlineLISTV.pop_back();
		onlineLISTV.push_back(']');
	}

	const string& getPlayerListViews() {
		return onlineLISTV;
	}
}
int lb_oListV(lua_State* L) {
	lua_settop(L, 0);
	auto& s = onlinepl::getPlayerListViews();
	lua_pushlstring(L, s.data(), s.size());
	return 1;
}

int lb_llVersion(lua_State* L) {
	lua_settop(L, 0);
	lua_pushstring(L, Version.c_str());
	return 1;
}

int lb_oList(lua_State* L) {
	lua_settop(L, 0);
	auto ps = liteloader::getAllPlayers();
	lua_newtable(L);
	int idx = 0;
	for (auto i : ps) {
		lua_pushinteger(L, (uintptr_t)i);
		lua_rawseti(L, -2, ++idx);
	}
	return 1;
}

int lb_bctext(lua_State* L) {
	//broadcast text
	try {
		string_view cont(LReadStr(L, 1));
		TextType tp{ RAW };
		if (lua_gettop(L)) {
			lua_pop(L, *(int*)&tp);
		}
		auto pls = liteloader::getAllPlayers();
		for (auto i : pls) {
			WPlayer(*i).sendText(cont.data(), tp);
		}
		return 0;
	}
	CATCH()
}

int lb_getpos(lua_State* L) {
	try {
		if (lua_gettop(L) != 2) throw "getPos(pl)"s;
		auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
		//x y z dim
		lua_pop(L, 1);
		auto& pos = ac->getPos();
		lua_pushnumber(L, pos.x);
		lua_pushnumber(L, pos.y);
		lua_pushnumber(L, pos.z);
		lua_pushinteger(L, ac->getDimensionId());
		return 4;
	}
	CATCH()
}

int lb_getXuid(lua_State* L) {
	try {
		auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
		if (isplayer(ac)) {
			lua_pushstring(L, offPlayer::getXUIDString((Player*)ac).c_str());
			return 1;
		}
	}CATCH()
}
int lb_Name2Xuid(lua_State* L) {
	try {
		string str = lua_tostring(L, 2);
		auto xid = XIDREG::str2id(str);
		if (!xid.set) {
			throw "not a player"s;
		}
		auto xuid = xid.val();
		lua_pushstring(L, std::to_string(xuid).c_str());
		return 1;
	}
	CATCH()
}

int lb_Xuid2Name(lua_State* L) {
	try {
		string xuid = lua_tostring(L, 2);
		xuid_t xid = std::stoull(xuid);
		auto name = XIDREG::id2str(xid);
		if (!name.set) {
			throw "not a xuid"s;
		}
		lua_pushstring(L, name.val().c_str());
		return 1;
	}
	CATCH()
}
int lb_getName(lua_State* L) {
	try {
		auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
		if (isplayer(ac)) {
			lua_pushstring(L, offPlayer::getRealName((Player*)ac).c_str());
			return 1;
		}
		lua_pushstring(L, ac->getNameTag().c_str());
		return 1;
	}CATCH()
}

int lb_tp(lua_State* L) {
	try {
		if (lua_gettop(L) != 6) throw "teleport(pl)"s;
		auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
		auto x = (float)lua_tointeger(L, 3);
		auto y = (float)lua_tointeger(L, 4);
		auto z = (float)lua_tointeger(L, 5);
		auto dim = lua_tointeger(L, 6);
		(WActor(*ac)).teleport({ x,y,z }, dim);
	}CATCH()
		return 0;
}

int lb_getEntityTypeId(lua_State* L) {
	auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushinteger(L, ac->getEntityTypeId());
	return 1;
}

int lb_getFullName(lua_State* L) {
	auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
	string name = "";
	if (ac) {
		auto name = SymCall("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z", string, int, int)(ac->getEntityTypeId(), 1);
	}
	lua_pushstring(L, name.c_str());
	return 1;
}

int lb_kill(lua_State* L) {
	auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
	SymCall("?kill@Mob@@UEAAXXZ", void, void*)(ac);
	return 0;
}

int lb_forceKick(lua_State* L) {
	auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
	auto l = offPlayer::getLevel(ac);
	SymCall("?forceRemoveEntity@Level@@UEAAXAEAVActor@@@Z",
		void, void*, void*)(l, ac);
	return 0;
}

addApi(lb_getHand) {
	auto ac = (Player*)(uintptr_t)lua_tointeger(L, 2);
	if (isplayer(ac)) {
		lua_pushinteger(L, (uintptr_t) & (ac->getSelectedItem()));
		return 1;
	}
}

using VarULong = VarInts<unsigned long long>;
int vanish(lua_State* L) {
	try {
		auto argnum = lua_gettop(L);
		if (argnum > 3)throw "vanish(ac)"s;
		auto ac = (Actor*)(uintptr_t)lua_tointeger(L, 2);
		VarULong ul(ZigZag(ac->getUniqueID().id));
		WBStream ws;
		ws.apply(ul);
		MyPkt<14> pk{ ws };
		if (argnum == 3) {
			auto pl = (ServerPlayer*)(uintptr_t)lua_tointeger(L, 3);
			if (isplayer(pl)) {
				pl->sendNetworkPacket(pk);
			}
			return 1;
		}
		vector<Player*> plist = liteloader::getAllPlayers();
		for (auto p : plist) {
			if (p != ac) {
				auto spp = (ServerPlayer*)p;
				spp->sendNetworkPacket(pk);
			}
		}
	} CATCH()
		return 0;
}

int lb_getPlayerInv(lua_State* L) {
	auto pl = (uintptr_t)lua_tointeger(L, 2);
	lua_newtable(L);
	auto v9 = *((uintptr_t*)pl + 380);
	int idx = 0;
	auto v13 = (*(__int64(__fastcall**)(uintptr_t))(**(uintptr_t**)(v9 + 176) + 112i64))(*(uintptr_t*)(v9 + 176));
	for (int i = 0; i < v13; ++i)
	{
		auto v15 = (*(__int64(__fastcall**)(uintptr_t, uintptr_t))(**(uintptr_t**)(v9 + 176) + 40i64))(
			*(uintptr_t*)(v9 + 176),
			(unsigned int)i);
		lua_pushinteger(L, ++idx);
		lua_pushinteger(L, v15);
		lua_settable(L, -3);
	}
	return 1;
}

int setServerOnlinePlayer(lua_State* L) {
#define _BYTE char
#define _QWORD uintptr_t
	auto now = lua_tointeger(L, 1);
	auto max = lua_tointeger(L, 2);
	auto v1 = (uintptr_t)mc->getServerNetworkHandler();
	if (*((_BYTE*)v1 + 584))
	{
		auto v2 = 0;
		auto v8 = *((_QWORD*)v1 + 10);
		auto v9 = *(void(__fastcall**)(__int64, char*, __int64, _QWORD, int, int, char))(*(_QWORD*)v8 + 24i64);
		auto v10 = *((int*)v1 + 180);
		auto v11 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)v1 + 7) + 1136i64))(*((_QWORD*)v1 + 7));
		auto v12 = SymCall("?getGameType@LevelData@@QEBA?AW4GameType@@XZ",
			char, uintptr_t)(v11);
		auto v13 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)v1 + 7) + 1136i64))(*((_QWORD*)v1 + 7));
		auto v14 = SymCall("?getLevelName@LevelData@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			__int64, uintptr_t)(v13);
		v9(v8, (char*)v1 + 600, v14, v12, now, max, 1);
	}
	return 0;
}

addApiEx(lb_spawnParticle, {
	std::string type = lua_tostring(L, 1);
	auto x = lua_tonumber(L,2);
	auto y = lua_tonumber(L, 3);
	auto z = lua_tonumber(L, 4);
	auto dimid = lua_tointeger(L, 5);
	auto level = mc->getLevel();
	//std::cout << "x:" << x << " y:" << y << " z:" << z << " dimid:" << dimid << std::endl;
	auto dim = SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
		Dimension*, void*,AutomaticID<Dimension,int>)(level, AutomaticID<Dimension, int>(dimid));
	SymCall("?spawnParticleEffect@Level@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVVec3@@PEAVDimension@@@Z",
		void, Level*, string&, const Vec3&, void*)
		(level, type, { (float)x,(float)y,(float)z }, dim);
	})

	static const luaL_Reg R_Actor[] = {
		{"getIP",[](lua_State* L) {
				try {
					auto pl = (Player*)(uintptr_t)lua_tointeger(L, 1);
					if (isplayer((void*)pl)) {
						lua_pushstring(L, liteloader::getIP(*offPlayer::getNetworkIdentifier(pl)).c_str());
						return 1;
					}
				}CATCH()
				}},
		{"isOP",[](lua_State* L) {
				try {
					auto pl = (void*)(uintptr_t)lua_tointeger(L, 1);
					if (!pl) {
						lua_pushboolean(L, true);
						return 1;
					}
					if (isplayer(pl)) {
						WPlayer wp = WPlayer(*(ServerPlayer*)pl);
						lua_pushboolean(L, wp->getCommandPermissionLevel() != 0);
						return 1;
					}
				}CATCH()
			}},
		{"getPos", lb_getpos},
		{"getName", lb_getName},
		{"getXuid", lb_getXuid},
		{"teleport", lb_tp},
		{"sendText", lb_sendText},
		{"runCmdAs", lb_runcmdAs},
		{"getEntityTypeId",lb_getEntityTypeId},
		{"vanish",vanish},
		{"forceKick",lb_forceKick},
		{"kill",lb_kill},
		{"getHand",lb_getHand},
		{"getPlayerInv",lb_getPlayerInv},
		{"str2xid",lb_Name2Xuid},
		{"xid2str",lb_Xuid2Name},
		{"getFullName",lb_getFullName},
		{ NULL,NULL }
	};

	LModule luaBase_module() {
		registerLuaModule(LModule([](lua_State* L) {
			lua_newtable(L);
			luaL_setfuncs(L, R_Actor, 0);
			lua_setglobal(L, "Actor");
			}));
		return LModule{ [](lua_State* L) {
			lua_register(L, "bcText", lb_bctext);
			lua_register(L, "runCmd", lb_runcmd);
			lua_register(L, "runCmdEx", lb_runcmdex);
			lua_register(L, "oList", lb_oList);
			lua_register(L, "oListV", lb_oListV);
			lua_register(L, "lllVersion", lb_llVersion);
			lua_register(L, "setOnlineNum", setServerOnlinePlayer);
			lua_register(L, "spawnParticle", lb_spawnParticle);
			lua_register(L, "TSize", [](lua_State* L) {
				if (lua_type(L, 1) != LUA_TTABLE) {
					luaL_error(L, "table required in TSize");
					return 0;
				}
				lua_pushnil(L);
				int c = 0;
				while (lua_next(L, 1)) {
					c++;
					lua_pop(L, 1);
				}
				lua_settop(L, 0);
				lua_pushinteger(L, c);
				return 1;
			});
			lua_register(L, "makeCommand", [](lua_State* L) {
				auto argnum = lua_gettop(L);
				const char* cmd = lua_tostring(L, 1);
				const char* des = lua_tostring(L, 2);
				auto lv = 0;
				if (argnum == 4) {
				   lv = lua_tointeger(L, 3);
				}
				cmds[cmd] = std::make_pair(des, lv);
				return 0;
				});
		} };
	}

	THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ",
		void* self) {
		original(self);
		mc = (Minecraft*)self;
	}
