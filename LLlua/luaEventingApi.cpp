#include"pch.h"
#include <mc/Actor.h>
#include"luaBindings.h"
#include<unordered_set>
enum EVENTS_TYPE {
	ONJOIN = 0,
	ONLEFT = 1,
	ONCMD = 2,
	ONLCMD = 3,
	ONCHAT = 4,
	ONKILLMOB = 5,
	ONMOBHURT = 6,
	ONBLOCKDESTORY = 7,
	ONPLACEBLOCK = 8,
	ONUSEITEM = 9,
	ONOPENBARREL = 10,
	ONOPENCHEST = 11,
	ONCLOSEBARREL = 12,
	ONCLOSECHEST = 13,
	ONATTACK = 14,
	ONRESPAWN = 15,
	ONExplode = 16,
	ONTAKE = 17,
	ONDROP = 18,
	EVENTS_COUNT = 19
};
#include<vector>
#include <mc\Block.h>
using std::vector;
static vector<int> listeners[EVENTS_COUNT];
const static unordered_map<string_view, int> EVENT_IDS = {
	{"onJoin",ONJOIN},
	{"onLeft",ONLEFT},
	{"onCMD",ONCMD},
	{"onLCMD",ONLCMD},
	{"onChat",ONCHAT},
	{"onMobDie",ONKILLMOB},
	{"onMobHurt",ONMOBHURT},
	{"onPlayerDestroyBlock",ONBLOCKDESTORY},
	{"onPlayerPlaceBlock",ONPLACEBLOCK},
	{"onPlayerUseItem",ONUSEITEM},
	{"onPlayerOpenChest",ONOPENCHEST},
	{"onPlayerOpenBarrel",ONOPENBARREL},
	{"onPlayerCloseChest",ONCLOSECHEST},
	{"onPlayerCloseBarrel",ONCLOSEBARREL},
	{"onPlayerAttack",ONATTACK},
	{"onPlayerRespawn",ONRESPAWN},
	{"onExplode",ONExplode},
	{"onPlayerTakeItem",ONTAKE},
	{"onPlayerDropItem",ONDROP}
};
int lb_Listen(lua_State* L) {
	string_view ev(LReadStr(L, 1));
	auto it = EVENT_IDS.find(ev);
	if (it == EVENT_IDS.end()) {
		luaL_error(L, "cant find event");
		return 0;
	}
	int LREF = luaL_ref(L, LUA_REGISTRYINDEX);
	listeners[it->second].push_back(LREF);
	return 0;
}
int lb_cancel(lua_State* L) {
	luaL_unref(L, LUA_REGISTRYINDEX, lua_tointeger(L, 1));
	return 0;
}
template<typename... TP>
bool CallEventEx(EVENTS_TYPE type, TP&&... arg)
{
	LuaFly fly(_L);
	LuaStackBalance B(_L);
	int EHIDX(getEHIDX());
	for (int lid : listeners[type]) {
		//-1:func
		lua_rawgeti(_L, LUA_REGISTRYINDEX, lid);
		fly.pushs(std::forward<TP>(arg)...);
		if (lua_pcall(_L, sizeof...(arg), 1, EHIDX) != 0) {
			printf("[LUA] callevent error : %s\n", lua_tostring(_L, -1));
			return true;
		}
		if (lua_isinteger(_L, -1)) {
			long long x;
			fly.pop(x);
			if (x == -1) {
				return false;
			}
		}
	}
	return true;
}

bool oncmd_luacmd(CommandOrigin const& ori, CommandOutput& outp, CommandMessage& m) {
	string msg = m.get(ori);
	auto pos = msg.find(' ');
	string_view PRE = string_view(msg).substr(0, pos);
	string_view NEX;
	if (pos != PRE.npos)
		NEX = string_view(msg).substr(pos + 1);
	CallEventEx(ONLCMD, ori.getName(), PRE, NEX);
	return true;
}
void initEvents() {
	Event::addEventListener([](JoinEV ev) {
		CallEventEx(ONJOIN, (uintptr_t)ev.Player);
		});
	Event::addEventListener([](LeftEV ev) {
		CallEventEx(ONLEFT, (uintptr_t)ev.Player);
		});
	Event::addEventListener([](ChatEV ev) {
		if (!CallEventEx(ONCHAT, (uintptr_t)ev.pl, ev.msg))
			return false;
		return true;
		});
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("l", "use lua command", 0);
		CmdOverload(l, oncmd_luacmd, "cmd");
		});
}
LModule luaEvent_module() {
	return LModule{ [](lua_State* L)->void {
		static bool inited = false;
		if (!inited) {
			inited = true;
			initEvents();
		}
		lua_register(L, "Listen2", lb_Listen);
		lua_register(L, "__Cancel", lb_cancel);
	},[](lua_State*) {
		for (auto& i : listeners) {
			i.clear();
		}
	} };
}

THook(bool, "?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z",
	MinecraftCommands* _this, unsigned int* a2, std::shared_ptr<CommandContext> x, char a4) {
	Player* sp = MakeSP(x->getOrigin());
	string cmd = x->getCmd();
	if (cmd.at(0) == '/') {
		cmd = cmd.substr(1, cmd.size() - 1);
	}
	if (!CallEventEx(ONCMD, (uintptr_t)sp, cmd))
		return false;
	return original(_this, a2, x, a4);

}

THook(bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, Actor* pl, BlockPos* pos, ItemStack* a3, bool a4) {
	auto bl = SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", Block*, BlockSource*, BlockPos*)(_this, pos);
	int dim = pl->getDimensionId();
	if (!CallEventEx(ONBLOCKDESTORY, (uintptr_t)pl, (uintptr_t)bl, pos->x, pos->y, pos->z, dim))
		return false;
	return original(_this, pl, pos, a3, a4);
}

THook(bool, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z",
	Mob* self, void* ads) {
	char v83;
	auto v6 = *(void**)(*(__int64(__fastcall**)(void*, char*))(*(uintptr_t*)ads + 64i64))(ads, &v83);
	auto level = offPlayer::getLevel(self);
	auto ac = SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z"
		, Actor*, Level*, void*, bool)(level, v6, 0);
	if (!CallEventEx(ONKILLMOB, (uintptr_t)self, (uintptr_t)ac))
		return false;
	return original(self, ads);
}

THook(bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z",
	Mob* self, void* ads, int a1, bool a2, bool a3) {
	char v83;
	auto v6 = *(void**)(*(__int64(__fastcall**)(void*, char*))(*(uintptr_t*)ads + 64i64))(ads, &v83);
	auto level = offPlayer::getLevel(self);
	auto ac = SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z"
		, Actor*, Level*, void*, bool)(level, v6, 0);
	if (!CallEventEx(ONMOBHURT, (uintptr_t)self, (uintptr_t)ac))
		return false;
	return original(self, ads, a1, a2, a3);
}

THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* bs, Block* bl, BlockPos* a3, unsigned __int8 a4, Actor* pl, bool a6) {
	if (pl) {
		auto v9 = *((void**)pl + 380);;
		(int)((*((uintptr_t*)bs + 29) - *((uintptr_t*)bs + 28)) / 144);
		int dim = pl->getDimensionId();
		if (!CallEventEx(ONPLACEBLOCK, (uintptr_t)pl, (uintptr_t)bl, a3->x, a3->y, a3->z, dim))
			return false;
	}
	return original(bs, bl, a3, a4, pl, a6);
}

THook(bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* _this, ItemStack* item, BlockPos* bp, unsigned __int8 a4, Vec3* a5, Block* block) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(_this) + 8);
	int dim = sp->getDimensionId();
	auto bll = dAccess<BlockLegacy*, 16>(block);
	string blockname = dAccess<string, 128>(bll);
	if (!CallEventEx(ONUSEITEM, (uintptr_t)sp, (uintptr_t)item, blockname, bp->x, bp->y, bp->z, dim))
		return false;
	return original(_this, item, bp, a4, a5, block);
}

THook(bool, "?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	void* _this, Player* pl, BlockPos* bp) {
	int dim = pl->getDimensionId();
	if (!CallEventEx(ONOPENCHEST, (uintptr_t)pl, bp->x, bp->y, bp->z, dim))
		return false;
	return original(_this, pl, bp);
}

THook(bool, "?use@BarrelBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	void* _this, Player* pl, BlockPos* bp) {
	int dim = pl->getDimensionId();
	if (!CallEventEx(ONOPENBARREL, (uintptr_t)pl, bp->x, bp->y, bp->z, dim))
		return false;
	return original(_this, pl, bp);
}

THook(bool, "?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z",
	void* _this, Player* pl) {
	auto bp = (BlockPos*)((uintptr_t*)_this - 204);
	int dim = pl->getDimensionId();
	if (!CallEventEx(ONCLOSECHEST, (uintptr_t)pl, bp->x, bp->y, bp->z, dim))
		return false;
	return original(_this, pl);
}

THook(bool, "?stopOpen@BarrelBlockActor@@UEAAXAEAVPlayer@@@Z",
	void* _this, Player* pl) {
	auto bp = (BlockPos*)((uintptr_t*)_this - 204);
	int dim = pl->getDimensionId();
	if (!CallEventEx(ONCLOSEBARREL, (uintptr_t)pl, bp->x, bp->y, bp->z, dim))
		return false;
	return original(_this, pl);
}

THook(bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z",
	Player* pl,  Actor* ac,   void* a3) {
	if (!CallEventEx(ONATTACK, (uintptr_t)pl, (uintptr_t)ac))
		return false;
	return original(pl, ac,a3);
}

/*THook(bool, "?respawn@Player@@UEAAXXZ",
	Player* pl) {
	if (!CallEventEx(ONRESPAWN, (uintptr_t)pl))
		return false;
	return original(pl);
}*/

THook(bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
	if (!CallEventEx(ONDROP, (uintptr_t)_this, (uintptr_t)a2))
		return false;
	return original(_this, a2, a3);
}


THook(bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z",
	Player* _this, Actor* a1, int a2, int a3) {
	if (!CallEventEx(ONTAKE, (uintptr_t)_this, (uintptr_t)a1))
		return false;
	return original(_this, a1, a2, a3);
}



THook(bool, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* bs, Actor* a3, Vec3 pos, float a5, bool a6, bool a7, float a8, bool a9) {
	auto dimid = dAccess<int, 216>(dAccess<void*, 32>(bs));
	if (!CallEventEx(ONExplode, (uintptr_t)a3, pos.x, pos.y, pos.z, dimid))
		return false;
	return original(_this, bs, a3, pos, a5, a6, a7, a8, a9);
}
