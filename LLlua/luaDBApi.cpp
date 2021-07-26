#include"pch.h"
#include"luaBindings.h"
unique_ptr<KVDBImpl> db;
int lb_dbget(lua_State* L) {
	try {
		LuaFly fly{ L };
		string rv;
		string mainkey;
		xstring key;
		fly.pops(key, mainkey);
		auto has = db->get(mainkey + "-" + key, rv);
		if (has)
			fly.push(rv);
		else
			lua_pushnil(L);
		return 1;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}
int lb_dbput(lua_State* L) {
	try {
		LuaFly fly{ L };
		string mainkey;
		xstring key;
		xstring cont;
		fly.pops(cont, key, mainkey);
		db->put(mainkey + "-" + key, cont);
		return 0;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}
int lb_dbdel(lua_State* L) {
	try {
		LuaFly fly{ L };
		string mainkey;
		xstring key;
		fly.pops(key, mainkey);
		db->del(mainkey + "-" + key);
		return 0;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}
int lb_dbforeach(lua_State* L) {
	try {
		LuaFly fly{ L };
		xstring prefix,cb;
		fly.pops(cb,prefix);
		bool flag = false;
		db->iter([prefix,&fly,cb,&flag](string_view k, string_view v) {
			if (k._Starts_with(prefix)) {
				flag = true;
				fly.Calll(cb.c_str(), 0, k, v);
			}
			else {
				if (flag) return false;
			}
			return true;
		});
		return 0;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}
int lb_dbremove_prefix(lua_State* L) {
	try {
		LuaFly fly{ L };
		xstring prefix, cb;
		fly.pops(cb, prefix);
		bool flag = false;
		vector<string> to_del;
		db->iter([prefix,&flag,&to_del](string_view k) {
			if (k._Starts_with(prefix)) {
				flag = true;
				to_del.emplace_back(prefix);
			}
			else {
				if (flag) return false;
			}
			return true;
		});
		for (auto& i : to_del) {
			db->del(i);
		}
		return 0;
	}
	catch (string e) {
		luaL_error(L, e.c_str());
		return 0;
	}
}

bool oncmd_dumpdb(CommandOrigin const& ori, CommandOutput& outp, string& target) {
	bool flag = false;
	db->iter([&](string_view k) {
		if (k._Starts_with(target)) {
			flag = true;
			string v;
			db->get(k, v);
			outp.addMessage(string{ k.substr(target.size()) }+" = " + v);
		}
		else {
			if (flag) return false;  //db->iter's key is ordered
		}
		return true;
		});
	return true;
}
bool oncmd_deldb(CommandOrigin const& ori, CommandOutput& outp, string& target) {
	bool flag = false;
	vector<string> to_del;
	db->iter([&](string_view k) {
		if (k._Starts_with(target)) {
			to_del.emplace_back(k);
			outp.addMessage(string{ k.substr(target.size()) }+" deleted");
		}
		else {
			if (flag) return false;  //db->iter's key is ordered
		}
		return true;
		});
	for (auto& i : to_del) {
		db->del(i);
	}
	return true;
}

void luaDB_entry(lua_State* L) {
	if (!db.get())
	{
		db = MakeKVDB(GetDataPath("luadb"), true, 2);
		Event::addEventListener([](RegCmdEV ev) {
			CMDREG::SetCommandRegistry(ev.CMDRg);
			MakeCommand("lua_db", "dump lua db", 1);
			CmdOverload(lua_db, oncmd_dumpdb, "prefix");
			MakeCommand("lua_db_del", "del lua db", 1);
			CmdOverload(lua_db_del, oncmd_deldb, "prefix");
		});
	}
	lua_register(L, "dget", lb_dbget);
	lua_register(L, "ddel", lb_dbdel);
	lua_register(L, "dput", lb_dbput);
	lua_register(L, "dforeach", lb_dbforeach);
	lua_register(L, "ddel_prefix", lb_dbremove_prefix);
}
LModule luaDB_module() {
	return { luaDB_entry };
}