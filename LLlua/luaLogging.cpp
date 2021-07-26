#include"pch.h"
#include"luaBindings.h"
Logger LOG(stacked{stdio_commit("[LUALOG] "),file_commit("plugins\\LiteLuaLoader\\lualog.log") });
int lb_log(lua_State* L) {
	try {
		int n = lua_gettop(L);
		LuaFly lf{ L };
		string out;
		for (int i = 1; i <= n; ++i) {
			xstring now;
			lf.readx(now, i);
			out.append(now);
			out.push_back(' ');
		}
		if (out.size()) out.pop_back();
		LOG(out);
		return 0;
	}CATCH()
}
LModule lualog_module() {
	return LModule{ [](lua_State* L) {
		lua_register(L,"Log",lb_log);
	} };
}