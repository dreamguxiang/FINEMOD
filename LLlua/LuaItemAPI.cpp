#include"pch.h"
#include"luaBindings.h"
#include <mc/Item.h>

addApi(Item_getId) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushinteger(L, item->getId());
	return 1;
}

addApi(Item_getAuxId) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushinteger(L, item->getAuxValue());
	return 1;
}

addApi(Item_getCustomName) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushstring(L, item->getCustomName().c_str());
	return 1;
}

addApi(Item_getName) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushstring(L, item->getName().c_str());
	return 1;
}
addApi(Item_getFullName) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	string mstr2 = "";
	SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		void, Item const*, string*)(item->getItem(), &mstr2);
	//std::cout << "<Debug> "<<mstr2 << std::endl;
	lua_pushstring(L, mstr2.c_str());
	return 1;
}

addApi(Item_isNull) {
	auto item = (ItemStack*)(uintptr_t)lua_tointeger(L, 2);
	lua_pushboolean(L, item->isNull());
	return 1;
}

static const luaL_Reg R_Item[] = {
	{"getName",Item_getName},
	{"getFullName",Item_getFullName},
	{"getCustomName",Item_getCustomName},
	{"getId",Item_getId},
	{"getAuxId",Item_getAuxId},
	{"isNull",Item_isNull},
	{ NULL,NULL }
};

void initItemM() {
	registerLuaModule(LModule([](lua_State* L) {
		lua_newtable(L);
		luaL_setfuncs(L, R_Item, 0);
		lua_setglobal(L, "Item");
		}));
}
