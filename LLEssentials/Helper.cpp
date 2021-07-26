#include "pch.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include "commad.h"
#include "tpa.h"
#include <api/scheduler/scheduler.h>
using namespace std::filesystem;
Logger1 LOG1("./liteloader.log");
unordered_map<string, string> CMDMAP, CMDSCHEDULE;
int FAKE_SEED, MAX_CHAT_LEN, MAXONLINEPLAYER, FAKE_ONLINEPLAYER, dpNUM, dpnowNUM, REDSTONETICK, TPRNUM;
unordered_set<short> logItems, banItems;
bool LOG_CMD, LOG_CHAT, regABILITY, NO_EXPLOSION, nocheatcmd,Force_ResourcePacks,EXP_PLAY, penderman, pfarm, fakeonlineplayer , ENABLETPR, ENABLERUNAS, DynamicPlayer, ENABLETPS,ENABLEBAN, ENABLESKICK, ENABLELORE, ENABLEVANISH, ENABLECNAME, ENABLETRANSFER, ENABLEGMODE;
Minecraft* mc;
const string version = u8"FINE特供";

void Version() {
	cout << "[LLEssentials] version "<< version << endl;
}
void loadCfg() {
	create_directory("plugins\\LLEssentials");
	 try {
		CMDMAP.clear();
		ConfigJReader jr("plugins/LLEssentials/LLHelper.json");
		jr.bind("CMDMAP", CMDMAP);
		jr.bind("timer", CMDSCHEDULE);
		jr.bind("FAKE_SEED", FAKE_SEED, 114514);
		jr.bind("NO_EXPLOSION", NO_EXPLOSION, false);
		jr.bind("MAX_CHAT_LEN", MAX_CHAT_LEN, 96);
		jr.bind("LOG_CHAT", LOG_CHAT, true);
		jr.bind("LOG_CMD", LOG_CMD, true);
		jr.bind("NoEndermanTakeBlock", penderman, true);
		jr.bind("ProtectFarmBlock", pfarm, true);
		jr.bind("nocheatcmd", nocheatcmd, true);
		jr.bind("RegAbility", regABILITY, true);
		jr.bind("Force_ResourcePacks", Force_ResourcePacks, false);
		jr.bind("EnableBan", ENABLEBAN, true);
		jr.bind("EnableSkick", ENABLESKICK, true);
		jr.bind("EnableVanish", ENABLEVANISH, true);
		jr.bind("EnableCname", ENABLECNAME, true);
		jr.bind("EnabletTransfer", ENABLETRANSFER, true);
		jr.bind("EnableGmode", ENABLEGMODE, true);
		jr.bind("EnableLore", ENABLELORE, true);
		jr.bind("EnableRunAs", ENABLERUNAS, true);
		jr.bind("EnableTps", ENABLETPS, false);
		jr.bind("EnableTpr", ENABLETPR, true);
		jr.bind("TPR_NUM", TPRNUM,50000);
		jr.bind("RedstoneTick", REDSTONETICK, 1);
		vector<int> items;
		logItems.clear();
		banItems.clear();
		jr.bind("logItems", items, {});
		for (auto i : items) logItems.insert(i);
		items.clear();
		jr.bind("banItems", items, {});
		for (auto i : items) banItems.insert(i);
	}
	catch (string e) {
		printf("[LLEssentials] json error %s\n", e.c_str());
		throw 0;
	}
}
void bctext(string text, TextType a) {
	auto pls = liteloader::getAllPlayers();
	for (auto i : pls) {
		WPlayer(*i).sendText(text, a);
	}
}

//string typemob = "!player,type=!armor_stand,type=!Arrow,type=!Boat,type=!bee,type=!Chest_minecart,type=!Donkey,type=!Elder_guardian,type=!Ender_crystal,type=!Ender_dragon,type=!Endermite,type=!Ender_pearl,type=!Evocation_illager,type=!Fox,type=!Hopper_minecart,type=!Horse,type=!Iron_golem,type=!Leash_knot,type=!Lightning_bolt,type=!Minecart,type=!Mooshroom,type=!Mule,type=!Ocelot,type=!Panda,type=!Painting,type=!Parrot,type=!Polar_bear,type=!Shulker,type=!Skeleton_horse,type=!Snow_golem,type=!Splash_potion,type=!Thrown_trident,type=!Tnt,type=!Tnt_minecart,type=!Turtle,type=!Villager,type=!Villager_V2,type=!Vindicator,type=!Wither,type=!Wither_skeleton,type=!Strider,type=!Xp_orb,name=\"\" ";
void clear_init() {
	bctext(u8"§b 清理开始", RAW);
	liteloader::runcmdEx("tp @e[type=zombie,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=skeleton,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=creeper,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=spider,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=zombie_pigman,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=slime,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=enderman,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=magma_cube,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=stray,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=husk,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=blaze,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=guardian,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=drowned,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=salmon,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=cod,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=pufferfish,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=tropicalfish,name=""] ~1 ~");
	liteloader::runcmdEx("tp @e[type=squid,name=""] ~1 ~");
	//
	liteloader::runcmdEx("kill @e[type=minecraft:ice_bomb, name = ""]");
	liteloader::runcmdEx("kill @e[type=xp_orb]");
	liteloader::runcmdEx("kill @e[type=falling_block]");
	liteloader::runcmdEx("kill @e[type=ender_pearl]");
	liteloader::runcmdEx("kill @e[type=fireworks_rocket]");
	liteloader::runcmdEx("kill @e[type=arrow]");
	liteloader::runcmdEx("kill @e[type=snowball]");
	liteloader::runcmdEx("kill @e[type=egg]");
	liteloader::runcmdEx("kill @e[type=fireball]");
	liteloader::runcmdEx("kill @e[type=wither_skull]");
	liteloader::runcmdEx("kill @e[type=wither_skull_dangerous]");
	liteloader::runcmdEx("kill @e[type=small_fireball]");
	liteloader::runcmdEx("kill @e[type=vex]");
	liteloader::runcmdEx("kill @e[type=fiercecraft:ghost]");
	liteloader::runcmdEx("kill @e[type=item]");
	bctext(u8"§b 清理结束", RAW);
}

void count_down() {
	std::thread t([] {
		bctext(u8"§b60秒后开始清扫生物与掉落物", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(30));
		bctext(u8"§b30秒后开始清扫生物与掉落物", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(20));
		bctext(u8"§b10秒后开始清扫生物与掉落物", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(5));
		bctext(u8"§b5秒后开始清扫生物与掉落物", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bctext(u8"§b4", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bctext(u8"§b3", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bctext(u8"§b2", RAW);
		std::this_thread::sleep_for(std::chrono::seconds(1));
		bctext(u8"§b1", RAW);
		clear_init();
		});
	t.detach();
}

void cleartime() {
	Handler::schedule(RepeatingTask([] {
		count_down();
		}, 1200));
}
void entry() {
	loadCfg();
	tpa_entry();
	REGCMD();
	Version();
	Event::addEventListener([](ServerStartedEV) {
		sschTask();
		cleartime;
		//if (DynamicPlayer) {
			//MAXONLINEPLAYER = 1;
			//FAKE_ONLINEPLAYER = 0;
			//setServerMotd();
			//updateCheck();
		//}
		});
}

THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ", void* self) {
	original(self);
	mc = (Minecraft*)self;
}

THook(void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z",
	void* self) {
	if (regABILITY)
	SymCall("?setup@AbilityCommand@@SAXAEAVCommandRegistry@@@Z"
		, void, void*)(self);
	return original(self);
}
/*
#define _BYTE char
#define _QWORD uintptr_t
THook(void, "?updateServerAnnouncement@ServerNetworkHandler@@QEAAXXZ",
	ServerNetworkHandler* self, __int64 a2) {
	original(self, a2);
	if (DynamicPlayer || fakeonlineplayer) {
		if (*((_BYTE*)self + 584))
		{
			auto v2 = 0;
			auto v3 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1664i64))(*((_QWORD*)self + 7));
			auto v4 = *(Actor***)(v3 + 8);
			for (auto i = *(Actor***)v3; i != v4; ++i)
			{
				if (!(*((unsigned __int8*)*i + 961)))
					++v2;
			}
			auto v8 = *((_QWORD*)self + 10);
			auto v9 = *(void(__fastcall**)(__int64, char*, __int64, _QWORD, int, int, char))(*(_QWORD*)v8 + 24i64);
			auto v10 = *((int*)self + 180);
			auto v11 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1136i64))(*((_QWORD*)self + 7));
			auto v12 = SymCall("?getGameType@LevelData@@QEBA?AW4GameType@@XZ",
				char, uintptr_t)(v11);
			auto v13 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1136i64))(*((_QWORD*)self + 7));
			auto v14 = SymCall("?getLevelName@LevelData@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
				__int64, uintptr_t)(v13);
			v9(v8, (char*)self + 600, v14, v12, v2, v10, 1);
		}
		return;
	}
	return original(self, a2);
}
*/
int tmp = 0;
THook(void, "?evaluate@CircuitSystem@@QEAAXPEAVBlockSource@@@Z", void* self,BlockSource* bs) {
	if (REDSTONETICK) {
		tmp++;
		if (tmp == REDSTONETICK) {
			tmp = 0;
			return original(self, bs);
		}
	}
}
/*
THook(bool, "?useItem@GameMode@@UEAA_NAEAVItemStack@@@Z",
	void* self, ItemStack* item) {
	std::string id = std::to_string(item->getId());
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(self) + 8);
	auto it = CMDMAP.find(id);
	WPlayer wp{ *sp };
	if (it != CMDMAP.end()) { wp.runcmd(it->second); return false; }
	return original(self, item);
}
*/

THook(string&, "?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void* x) {
	if (auto it = ORIG_NAME._map.find((ServerPlayer*)x); it != ORIG_NAME._map.end()) {
		return it->second;
	}
	return original(x);
}

