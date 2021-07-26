#include"pch.h"
#include"luaBindings.h"
#include<unordered_set>
#include<api/scheduler/scheduler.h>
using std::unordered_set;
static unordered_map<taskid_t,int> LUA_TASKS_REF;
static int lb_sch_cancel_ex(lua_State* L) {
	int n = lua_gettop(L);
	if (n != 1 || !lua_isinteger(L, 1)) {
		luaL_error(L, "cancel(taskid)");
		return 0;
	}
	taskid_t tid;
	tid = taskid_t(lua_tointeger(L, 1));
	auto it = LUA_TASKS_REF.find(tid);
	if (it != LUA_TASKS_REF.end()) {
		lua_pushboolean(L, Handler::cancel(tid));
		luaL_unref(L, LUA_REGISTRYINDEX, it->second);
	}
	return 1;
}
static int lb_schedule_ex(lua_State* L) {
	try {
		int n = lua_gettop(L);
		int delay = 0, interval = 0;
		interval = int(lua_tointeger(L, 2));
		if (n == 3) {
			delay = int(lua_tointeger(L, 3));
		}
		lua_settop(L, 1);
		int LUAREF=luaL_ref(L, LUA_REGISTRYINDEX);
		auto tid = Handler::schedule(DelayedRepeatingTask([LUAREF, interval]() {
			LuaStackBalance B(::_L);
			int EHIDX(getEHIDX());
			lua_rawgeti(::_L, LUA_REGISTRYINDEX, LUAREF);
			if (lua_pcall(::_L, 0, 0, EHIDX) != 0) {
				printf("[LUA] scheduler error : %s\n", lua_tostring(::_L, -1));
			}
			if (interval == 0) {
				luaL_unref(::_L, LUA_REGISTRYINDEX, LUAREF);
			}
		}, delay, interval));
		LUA_TASKS_REF[tid] = LUAREF;
		lua_settop(L, 0);
		lua_pushinteger(L, tid);
		return 1;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}
void lua_scheduler_reload(lua_State*) {
	for (auto [id,ref] : LUA_TASKS_REF) {
		Handler::cancel(id);
	}
}
static const luaL_Reg R[] =
{
	{ "schedule",	lb_schedule_ex	},
	{ "cancel",	lb_sch_cancel_ex	},
	{ NULL,		NULL	}
};
static void lua_sch_entry(lua_State* L) {
	lua_newtable(L);
	luaL_setfuncs(L, R, 0);
	lua_setglobal(L, "schapi");
}
LModule luaSch_module() {
	return { lua_sch_entry,lua_scheduler_reload };
}