#include "pch.h"
#include "pch.h"
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std::filesystem;
bool fakeonlineplayer , DynamicPlayer;
using namespace std;
int FAKE_ONLINEPLAYER, MAXONLINEPLAYER;
Minecraft* mc;
void loadCfg() {
	create_directory("plugins\\DynamicPlayer");
	try {
		ConfigJReader jr("plugins/DynamicPlayer/DynamicPlayer.json");
		jr.bind("fakeonlineplayer", fakeonlineplayer, false);
		jr.bind("DynamicPlayer", DynamicPlayer, false);
	}
	catch (string e) {
		printf("[LLEssentials] json error %s\n", e.c_str());
		throw 0;
	}
}
THook(void, "?initAsDedicatedServer@Minecraft@@QEAAXXZ", void* self) {
	original(self);
	mc = (Minecraft*)self;
}
void setServerMotd() {
#define _BYTE char
#define _QWORD uintptr_t
	auto v1 = (uintptr_t)mc->getServerNetworkHandler();
	if (*((_BYTE*)v1 + 592))
	{
		auto v2 = 0;
		auto v8 = *((_QWORD*)v1 + 11);
		auto v9 = *(void(__fastcall**)(__int64, char*, __int64, _QWORD, int, int, char))(*(_QWORD*)v8 + 24i64);
		auto v10 = *((int*)v1 + 182);
		auto v11 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)v1 + 7) + 1160i64))(*((_QWORD*)v1 + 7));
		auto v12 = SymCall("?getGameType@LevelData@@QEBA?AW4GameType@@XZ",
			char, uintptr_t)(v11);
		auto v13 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)v1 + 7) + 1160i64))(*((_QWORD*)v1 + 7));
		auto v14 = SymCall("?getLevelName@LevelData@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			__int64, uintptr_t)(v13);
		v9(v8, (char*)v1 + 608, v14, v12, FAKE_ONLINEPLAYER, MAXONLINEPLAYER, 1);
	}
}

THook(void, "?updateServerAnnouncement@ServerNetworkHandler@@QEAAXXZ",
	ServerNetworkHandler* self, __int64 a2) {
	original(self, a2);
	if (DynamicPlayer || fakeonlineplayer) {
		if (*((_BYTE*)self + 592))
		{
			auto v2 = 0;
			auto v3 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1712i64))(*((_QWORD*)self + 7));
			auto v4 = *(Actor***)(v3 + 8);
			for (auto i = *(Actor***)v3; i != v4; ++i)
			{
				if (!(*((unsigned __int8*)*i + 993)))
					++v2;
			}
			auto v8 = *((_QWORD*)self + 11);
			auto v9 = *(void(__fastcall**)(__int64, char*, __int64, _QWORD, int, int, char))(*(_QWORD*)v8 + 24i64);
			auto v10 = *((int*)self + 182);
			auto v11 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1160i64))(*((_QWORD*)self + 7));
			auto v12 = SymCall("?getGameType@LevelData@@QEBA?AW4GameType@@XZ",
				char, uintptr_t)(v11);
			auto v13 = (*(__int64(__fastcall**)(_QWORD))(**((_QWORD**)self + 7) + 1160i64))(*((_QWORD*)self + 7));
			auto v14 = SymCall("?getLevelName@LevelData@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
				__int64, uintptr_t)(v13);
			v9(v8, (char*)self + 608, v14, v12, v2, v10, 1);
		}
		return;
	}
	return original(self, a2);
}

bool oncmd_setPlNum(CommandOrigin const& ori, CommandOutput& outp, int num, int num1) {
	FAKE_ONLINEPLAYER = num;
	MAXONLINEPLAYER = num1;
	setServerMotd();
	if (num >= num1) {
		outp.error(u8"§6[§eDynamicPlayer§6]§r 当在线玩家大于最大玩家时，玩家将无法进入服务器，如果你要继续，请使用/setplnum_ op" );
		return false;
	}
	outp.success(u8"§6[§eDynamicPlayer§6]§r 设置成功");
	return true;
}

bool oncmd_setPlNumop(CommandOrigin const& ori, CommandOutput& outp, int num, int num1) {
	FAKE_ONLINEPLAYER = num;
	MAXONLINEPLAYER = num1;
	setServerMotd();
	outp.success(u8"§6[§eDynamicPlayer§6]§r 设置成功");
	return true;
}

void entry() {
	loadCfg();
	cout << "[DynamicPlayer] DynamicPlayer Loaded by wzy,DreamGuXiang, Build Date [" << __TIMESTAMP__ << u8"] ,support 1.16.210" << endl;
	if (DynamicPlayer) {
		MAXONLINEPLAYER = 1;
		FAKE_ONLINEPLAYER = 0;
	}
	Event::addEventListener([](RegCmdEV e) {
		CMDREG::SetCommandRegistry(e.CMDRg);
		MakeCommand("setplnum", "set fake online player num", 1);
		CmdOverload(setplnum, oncmd_setPlNum,"onlinenum","maxnum");
		MakeCommand("setplnum_op", "set fake online player num", 1);
		CmdOverload(setplnum_op, oncmd_setPlNumop, "onlinenum", "maxnum");
		});
	Event::addEventListener([](LeftEV var) {
		std::thread time([] {
			if (DynamicPlayer) {
				std::this_thread::sleep_for(std::chrono::nanoseconds(500));
				MAXONLINEPLAYER = MAXONLINEPLAYER - 1;
				FAKE_ONLINEPLAYER = FAKE_ONLINEPLAYER - 1;
				setServerMotd();
			}
			});
		time.detach();
		});
	Event::addEventListener([](JoinEV var) {
		if (DynamicPlayer) {
			MAXONLINEPLAYER = MAXONLINEPLAYER + 1;
			FAKE_ONLINEPLAYER = FAKE_ONLINEPLAYER + 1;
			setServerMotd();
		}
		});

	Event::addEventListener([](ServerStartedEV) {

		if (DynamicPlayer) {
			setServerMotd();
		}
		});
}