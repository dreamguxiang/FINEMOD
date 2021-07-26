#include"pch.h"
#include<litelualoader.h>
#include<lua.hpp>
#include<luafly.h>
#include"shared.h"
FastLand const* getFastLand_export(int x, int z, int dim);
string Vec2Str(vector<string> const& n) {
	string rv = "[\"";
	for (auto i : n) {
		rv.append(i);
		rv.append("\",\"");
	}
	if (rv.size() != 2) {
		rv.pop_back();
	}
	rv.back() = ']';
	return rv;
}
template<bool View = false>
int lb_getLand(lua_State* L) {
	try {
		//x y z dim
		double x, y, z;
		int dim;
		LuaFly lf{ L };
		lf.pops(dim, z, y, x);
		int xx = iround((float)x), zz = iround((float)z);
		auto fl = getFastLand_export(xx, zz, dim);
		if (!fl) {
			lua_pushnil(L);
			return 1;
		}
		int permo = fl->perm_others;
		int permg = fl->perm_group;
		vector<string> owners;
		for (u32 i = 0; i < fl->owner_sz; ++i) {
			optional<string> own = XIDREG::id2str(fl->owner[i]);
			if (own.set) {
				owners.emplace_back(std::move(own.val()));
			}
		}
		if constexpr (View) {
			lf.pushs(permo, permg, Vec2Str(owners));
		}
		else {
			lf.pushs(permo, permg, owners);
		}
		return 3;
	}CATCH()
}
void InitLUA() {
	registerLuaModule(LModule([](lua_State* L) {
		lua_register(L, "getLand", lb_getLand);
		lua_register(L, "getLandV", lb_getLand<true>);
		}));
}