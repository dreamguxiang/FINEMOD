#include"pch.h"
#include"luaBindings.h"
#if 0
ldata_t _lpget(int i) {
	if (lua_isinteger(L, i)) {
		return { lua_tointeger(L,i) };
	}
	else {
		if (lua_isstring(L, i)) {
			size_t ssz;
			auto str = lua_tolstring(L, i, &ssz);
			return { string{str,ssz} };
		}
		else {
			return { 0 };
		}
	}
}
BDXLUA_API unordered_map<string, luacb_t> bindings;
int lua_call_bind_proxy(lua_State* L) {
	auto n = lua_gettop(L);
	if (n < 1) {
		luaL_error(L, "lbind needs len(args)>=1,use lbind(\"name\",...)");
		return 0;
	}
	if (!lua_isstring(L, 1)) {
		lua_settop(L, 0);
		luaL_error(L, "use lbind(\"name\",...)");
		return 0;
	}
	auto str = lua_tostring(L, 1);
	auto it = bindings.find(str);
	if (it == bindings.end()) {
		lua_settop(L, 0);
		luaL_error(L, "binding %s not found!", str);
		return 0;
	}
	vector<ldata_t> arg;
	for (int i = 2; i <= n; ++i) {
		auto data = _lpget(i);
		arg.emplace_back(std::move(data));
	}
	lua_settop(L, 0);
	try {
		auto suc = it->second(arg);
		if (suc.index() == 0) {
			//int
			lua_pushinteger(L, std::get<0>(suc));
		}
		else {
			lua_pushlstring(L, std::get<1>(suc).data(), std::get<1>(suc).size());
		}
		return 1;
	}
	catch (string e) {
		luaL_error(L, "lbind error in %s : %s", str, e.c_str());
		return 0;
	}
	catch (...) {
		luaL_error(L, "lbind internal error in %s", str);
		return 0;
	}
	return 0;
}
#endif