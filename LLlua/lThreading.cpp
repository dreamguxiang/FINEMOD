#include"pch.h"
#include"luaBindings.h"
#include<thread>
#include<deque>
#include<mutex>
#include<api/scheduler/scheduler.h>
#include<stl/Bstream.h>
struct MQueue {
	using msgTP = std::pair<string, string>;
	std::deque<msgTP> msg;
	std::condition_variable not_empty;
	std::mutex lock;
	template<bool isback=true>
	void push(msgTP&& x) {
		std::unique_lock lk(lock);
		if constexpr (isback) {
			msg.emplace_back(std::forward<msgTP>(x));
		}
		else {
			msg.emplace_front(std::forward<msgTP>(x));
		}
		lk.unlock();
		not_empty.notify_one();
	}
	msgTP pop_block() {
		std::unique_lock lk(lock);
		while (msg.empty())
			not_empty.wait(lk);
		msgTP rv(std::move(msg.front()));
		msg.pop_front();
		lk.unlock();
		return rv;
	}
};
std::unordered_map<int,struct LThread> THREADS;
struct LThread {
		std::unique_ptr<std::thread> th;
		lua_State* S=nullptr;
		MQueue mq;
		int TID;
		int LuaRef;
		static int lua_SEND(lua_State* L) {
			int n = lua_gettop(L);
			WBStream ws;
			ws.data.reserve(256);
			for (int i = 1; i <= n; ++i) {
				if (lua_isinteger(L, i)) {
					BinVariant va({ lua_tointeger(L, i) });
					ws.apply(va);
				}
				else {
					if (lua_isstring(L, i)) {
						BinVariant va( { lua_tostring(L, i) });
						ws.apply(va);
					}
					else {
						luaL_error(L,"error type in SEND");
					}
				}
			}
			ws.apply(BinVariant{});
			lua_rawgetp(L, LUA_REGISTRYINDEX, (const void*)0x114514);
			Handler::scheduleNext([id = lua_tointeger(L, -1), M(std::move(ws.data))]{
				LuaStackBalance B(::_L);
				int EHIDX(getEHIDX());
				lua_rawgeti(::_L, LUA_REGISTRYINDEX, id);
				RBStream rs(M);
				int acnt = 0;
				while (1) {
					BinVariant bv;
					rs.apply(bv);
					if (bv.type == 0) break;
					acnt++;
					if (bv.type == 1) {
						lua_pushinteger(::_L, bv.v.x);
					}
					else {
						lua_pushlstring(::_L, bv.v.y.data(), bv.v.y.size());
					}
				}
				if (lua_pcall(::_L, acnt, 0, EHIDX) != 0) {
					printf("[lua error] MSG Handler %lld error:%s\n", id, lua_tostring(::_L, -1));
				}
			});
			lua_pop(L, 1);
			return 0;
		}
		bool init_env(const char* fn) {
			S = luaL_newstate();
			luaL_openlibs(S);
			lua_register(S, "TSize", [](lua_State* L) {
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
			lua_register(S, "SEND", lua_SEND);
			lua_pushinteger(S, LuaRef);
			lua_rawsetp(S, LUA_REGISTRYINDEX, (const void*)0x114514);
			luaL_loadfile(S, "plugins/LiteLuaLoader/lua/async/init_async.lua");
			lua_pcall(S,0,0,0);
			lua_getglobal(S, "EXCEPTION");
			int EHIDX = lua_gettop(S);
			luaL_loadfile(S, fn);
			if (lua_pcall(S, 0, 0, EHIDX) != 0) {
				printf("[lua error] in thread %d :%s\n", TID, lua_tostring(S, -1));
				return false;
			}
			lua_settop(S, 0);
			lua_getglobal(S, "EXCEPTION");
			//top =1
			return true;
		}
		void work_loop() {
			th = std::make_unique<std::thread>( [this] {while (1) {
				auto [param1, param2] = mq.pop_block();
				if (param1 == "!exit") {
					int tid = this->TID;
					Handler::schedule(DelayedTask([tid]{
						LThread::__term(tid);
					},5));
					return;
				}
				LuaStackBalance B(S);
				if (lua_getglobal(S, param1.c_str()) == 0) {
					printf("[lua thread %d] fn %s not found\n", TID,param1.c_str());
					continue;
				}
				RBStream rs(param2);
				int acnt = 0;
				while (1) {
					BinVariant bv;
					rs.apply(bv);
					if (bv.type == 0) break;
					acnt++;
					if (bv.type == 1) {
						lua_pushinteger(S, bv.v.x);
					}
					else {
						lua_pushlstring(S, bv.v.y.data(),bv.v.y.size());
					}
				}
				if (lua_pcall(S, acnt, 0, 1) != 0) {
					printf("[lua thread %d] EXCEPTION in fn %s:%s\n", TID,param1.c_str(), lua_tostring(S, -1));
				}
			}
			} 
			);
th->detach();
		}
		~LThread() {
			if(S)
				lua_close(S);
		}
		static void __term(int TID) {
			THREADS.erase(TID);
		}
		LThread(){}
		LThread(int){}
};
#include<random>
static int getTID(){
	std::random_device rdv;
	int rv=0;
	while (THREADS.count(rv)) rv = rdv();
	return rv;
}
int stopAll_Nowait() {
	for (auto& [k, v] : THREADS) {
		v.mq.push<false>({ "!exit",""});
	}
	return 0;
}
int l_StartThread(lua_State* L) {
	if (lua_gettop(L) != 2) {
		luaL_error(L, "error arg");
		return 0;
	}
	string_view cb(LReadStr(L,1));
	LuaFly lf(L);
	int tid = getTID();
	auto& thisthd = THREADS.emplace(tid,0).first->second;
	thisthd.TID = tid;
	thisthd.LuaRef= luaL_ref(L, LUA_REGISTRYINDEX);
	if (!thisthd.init_env(cb.data())) {
		THREADS.erase(tid);
		luaL_error(L, "cant start thread");
		return 0;
	}
	thisthd.work_loop();
	lua_settop(L, 0);
	lf.push(tid);
	return 1;
}
int l_StopThread(lua_State* L) {
	int tid;
	LuaFly lf(L);
	lf.pop(tid);
	auto it = THREADS.find(tid);
	if (it == THREADS.end()) {
		luaL_error(L, "cant find thread %d", tid);
		return 0;
	}
	it->second.mq.push<false>({"!exit",""});
	return 0;
}
int l_SendMsg(lua_State* L) {
	int tid;
	string call;
	tid = int(lua_tointeger(L, 1));
	call = lua_tostring(L, 2);
	int n = lua_gettop(L);
	WBStream ws;
	ws.data.reserve(256);
	for (int i = 3; i <= n; ++i) {
		if (lua_isinteger(L, i)) {
					BinVariant va({ lua_tointeger(L, i) });
					ws.apply(va);
				}
				else {
					if (lua_isstring(L, i)) {
						BinVariant va( { lua_tostring(L, i) });
						ws.apply(va);
					}
			else {
				luaL_error(L, "error type in TSendMsg");
			}
		}
	}
	ws.apply(BinVariant{});
	auto it = THREADS.find(tid);
	if (it == THREADS.end()) {
		luaL_error(L, "cant find thread %d", tid);
		return 0;
	}
	it->second.mq.push({std::move(call),std::move(ws.data)});
	return 0;
}
static void lt_entry(lua_State* L) {
	lua_register(L, "startThread", l_StartThread);
	lua_register(L, "stopThread", l_StopThread);
	lua_register(L, "TSendMsg", l_SendMsg);
}
static void lt_stop(lua_State* L) {
	stopAll_Nowait();
}
LModule luaThreading_module() {
	return { lt_entry,lt_stop };
}