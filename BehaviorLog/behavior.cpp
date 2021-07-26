#include <lbpch.h>
#include <mc/Player.h>
#include <stl\varint.h>
#include <mc/Core.h>
#include <mc/Actor.h>
#include <mc/Level.h>
#include <mc/Certificate.h>
#include <mc/BlockSource.h>
#include <mc/core.h>
#include <mc/mass.h>
#include <mc/BlockSource.h>
#include <mc/Block.h>
#include <mc/Item.h>
#include <stl\langPack.h>
#include <functional>
#include "sdkcaller.h"
#include "Helper.h"
#include "behavior.h"
#include <liteloader.h>
#pragma warning(disable:4996)
#pragma warning(disable:4067)




extern bool mode;
using namespace std;
typedef signed int  INT32, * PINT32;

extern bool logfd;
extern bool logfh;

extern bool logCmdMlock;
extern bool CmdBlockWriteable;
extern bool force_achievement;

extern std::string logfilec;
extern bool LogCMD;
extern bool LogJoin;
extern bool LogLeft;
extern bool LogChat;

extern bool mode;

extern bool LogUseItem;
extern bool LogChest;
extern bool LogPlace;
extern bool LogDestory;
extern bool LogCDim;

extern bool showLogUseItem;
extern bool showLogChest;
extern bool showLogBlock;
extern bool showLogCDim;
extern LangPack LangP;

using namespace std;
inline vector<pair<string,string>> LogList;
inline void addLog(const string& filen, const string& log) {
	LogList.push_back({ filen,log });
}
void wlog() {

	for (auto i : LogList) {

		fw(i.first, i.second);
	}
	LogList.clear();
}
void sync_logger() {
	for (;;) {
		wlog();
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
}

//#define IsDebugVer false;
#define NO_LISTENER
#ifdef NO_LISTENER
#define callListener(c) ;
#endif
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVCommandBlockUpdatePacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier* a1, /*CommandBlockUpdatePacket*/void* a2) {
#ifdef IsDebugVer
	cout << "handle@ServerNetworkHandler Called" << endl;
#endif
	ServerPlayer* sp = _this->_getServerPlayer(*a1, dAccess<unsigned char, 16>(a2));
	Vec3 pos;

	if (offCommandBlockUpdatePacket::isBlock(a2)) {
		BlockPos bpos = offCommandBlockUpdatePacket::getBlockPos(a2);
		pos = { (float)bpos.x,(float)bpos.y,(float)bpos.z };
	}
	else {
		pos = sp->getPos();
	}
	string cmd = offCommandBlockUpdatePacket::getCommand(a2);
	if (CmdBlockWriteable)
		original(_this, a1, a2);
	if (logCmdMlock) {
		logger::cmdbw("WriteCommandBlock", sp->getNameTag(), (int)sp->getDimensionId(), pos, cmd, CmdBlockWriteable ? _TRS("word.success") : _TRS("word.denied"));
	}
}

THook(bool,
	"?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* _this, ItemStack* item, const BlockPos* blkpos, unsigned __int8 a4, void* v5, Block* pBlk) {
#ifdef IsDebugVer
	cout << "?useItemOn@GameMode Called" << endl;
#endif
	bool vaule = original(_this, item, blkpos, a4, v5, pBlk); 
	
	if (LogUseItem) {
		Player* sp = offGameMode::getPlayer(_this);
		if (!item->isNull()) {
			std::string mstr = item->getName();

			INT32 pos[3] = { blkpos->x ,blkpos->y ,blkpos->z };

			if (mode) {
				if (vaule) {
					logger::Item("Operate", sp->getNameTag(), sp->getDimensionId(), _TRS("word.operate"), mstr, pos);
				}

			}
			mode = true;
		}
	}
	return vaule;
}
Block& Str2Block(string str) {
	string bstr = "?m" + str + "@VanillaBlocks@@3PEBVBlock@@EB";
	return **(Block**)dlsym_real(bstr.c_str());
}
class LevelContainerModel;
THook(void, "?containerContentChanged@LevelContainerModel@@UEAAXH@Z", LevelContainerModel* a1, int a2) {
#ifdef IsDebugVer
	cout << "?containerContentChanged@LevelContainerModel Called" << endl;
#endif

	if (LogChest) {
		Actor* v3 = dAccess<Actor*, 208>(a1);
		BlockSource* bs = offBlock::getBlockSource(v3);
		//offset LevelContainerModel::_getContainer(LevelContainerModel *this) Line->BlockSource::getBlockEntity
		BlockPos* bpos = offLevelContainerModel::getBlockPos(a1);
		Block* pBlk = SymBlockSource::getBlock(bs, bpos);
		auto v5 = (*(__int64(__fastcall**)(LevelContainerModel*))(*(uintptr_t*)a1 + 160i64))(a1);
		if (v5) {
			ItemStack* item = (ItemStack*)(*(__int64(__fastcall**)(__int64, uintptr_t))(*(uintptr_t*)v5 + 40i64))(v5, a2);
			auto size = offItemStack::getCount(item);
			string bname = offBlock::getFullName(pBlk);
			if (bname.find("minecraft:") != string::npos)
				bname = bname.replace(bname.find("minecraft:"), 10, "");
			if (!item->isNull()) {
				std::string object_name = item->getName();
				auto slot = a2;
				INT32 pos[3] = { bpos->x ,bpos->y ,bpos->z };
				if (size == 0) {
					logger::Container_Out("Event", v3->getNameTag(), bname, pos, v3->getDimensionId(), slot, object_name);
				}
				else {
					logger::Container_In("Event", v3->getNameTag(), bname, pos, v3->getDimensionId(), slot, size, object_name);
				}
			}
		}
	}
	return original(a1, a2);
}

THook(void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z", Mob* _this, ActorDamageSource* a2) {
#ifdef IsDebugVer
	cout << "die@Mob Called" << endl;
#endif
	if (LogDie)
		if (!_this->getNameTag().empty()) {
			string Mob_name = _this->getNameTag();
			string Mob_type_name = SymCommandUtils::getActorName(_this);
			auto level = offBlock::getLevel(_this);
			char v83;
			auto v6 = *(void**)(*(__int64(__fastcall**)(void*, char*))(*(uintptr_t*)a2 + 64))(a2, &v83);
			auto Src = SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z"
				, Actor*, Level*, void*, bool)(level, v6, 0);
			int srccode = dAccess<int>(a2, 8);
			string cause_n = SymActorDamageSource::lookupCauseName(srccode);
			string src_name, mob_name, Src_type_name, Src_name;
			if (Src) {
				Src_name = Src->getNameTag();
				Src_type_name = SymCommandUtils::getActorName(Src);
				if (!Mob_type_name.empty()) {
					if (Mob_name == Mob_type_name) {
						mob_name = Mob_name;
					}
					else {
						mob_name = Mob_name;
					}
				}
				if (!Src_type_name.empty()) {
					if (!Src_name.empty())
						src_name = Src_name;
					else
						src_name = Src_type_name;
				}
				else {
					src_name = "unknow";
				}
			}
			else {
				if (!Mob_type_name.empty()) {
					if (Mob_name == Mob_type_name) {
						mob_name = Mob_name;
					}
					else {
						mob_name = Mob_type_name;
					}
				}
				src_name = "unknow";
			}
			logger::showDie("DeathInfo", _this->getDimensionId(), _this->getPos(), src_name, mob_name, srccode);
		}
	original(_this, a2);
}


THook(bool, "?drop@Player@@UEAA_NAEBVItemStack@@_N@Z",
	Player* _this, ItemStack* a2, bool a3) {
#ifdef IsDebugVer
	cout << "drop@Player Called" << endl;
#endif
	if (LogTakeDrop) {
		logger::TakeDrop("Drop", _this->getNameTag(), _this->getDimensionId(), _TRS("word.drop"), a2->getName(), _this->getPos(), offItemStack::getCount(a2));
	}
	return original(_this, a2, a3);
}


THook(bool, "?take@Player@@QEAA_NAEAVActor@@HH@Z", Player* _this, Actor* a1, int a2, int a3) {
#ifdef IsDebugVer
	cout << "take@Player Called" << endl;
#endif
	bool vaule = original(_this, a1, a2, a3);
	if (vaule) {
		string itemname = SymCommandUtils::getActorName(a1);
		if (LogTakeDrop) {
			logger::TakeDrop("Take", _this->getNameTag(), _this->getDimensionId(), _TRS("word.take"), itemname, _this->getPos(), -1);
		}
	}
	
	return vaule;
}

THook(bool, "?playerWillDestroy@BlockLegacy@@UEBA_NAEAVPlayer@@AEBVBlockPos@@AEBVBlock@@@Z",
	BlockLegacy* _this, Player* pl, const BlockPos* blkpos, Block* bl) {
#ifdef IsDebugVer
	cout << "playerWillDestroy@BlockLegacy Called" << endl;
#endif
	bool vaule = original(_this, pl, blkpos, bl);
	if (LogDestory) {
		INT32 pos[3] = { blkpos->x ,blkpos->y ,blkpos->z };
		string bname = offBlock::getFullName(bl);

		if (vaule) {
			if (bname.find("minecraft:") != string::npos)
				bname = bname.replace(bname.find("minecraft:"), 10, "");
			logger::Block("Destroy", pl->getNameTag().c_str(), pl->getDimensionId(), _TRS("word.destroyed"), bname, pos);
		}
	}
	return vaule;
}


THook(__int64,
	"?onBlockPlacedByPlayer@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlock@@AEBVBlockPos@@_N@Z",
	void* _this, Player* pl, Block* blk, const BlockPos* blkpos, bool _bool) {
#ifdef IsDebugVer
	cout << "onBlockPlacedByPlayer Called" << endl;
#endif
	if (LogPlace) {
		mode = false;
		auto bname = offBlock::getFullName(blk);

		INT32 pos[3] = { blkpos->x ,blkpos->y ,blkpos->z };
		if (bname.find("minecraft:") != string::npos)
			bname = bname.replace(bname.find("minecraft:"), 10, "");
		logger::Block(
			"Place", pl->getNameTag().c_str(),  pl->getDimensionId(), _TRS("word.placed"),
			bname, pos);
	}
	return original(_this, pl, blk, blkpos, _bool);
}
THook(bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z", void* _this, Player* sp, void* req) {
#ifdef IsDebugVer
	cout << "_playerChangeDimension@Level Called" << endl;
#endif
	bool vaule = original(_this, sp, req);
	if (LogCDim) {
		if (vaule)
			logger::ChangeDimension("Dimension", sp->getNameTag().c_str(), sp->getDimensionId(), sp->getPos());
	}
	return vaule;

}
THook(bool, "?achievementsWillBeDisabledOnLoad@LevelData@@QEBA_NXZ", void* _this) {
	if(force_achievement)
		return false;
	return original(_this);
}

void Join() {
	Event::addEventListener([](JoinEV ev) {
		string logfile = getfilename(login);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< "Join"
			<< ev.Player->getNameTag()
			<< ev.Player->getDimensionId()
			<< "IP"
			<< ev.IP
			<< "xuid"
			<< ev.xuid;
		addLog(logfile, csv.getstr());
	});
}
void Left() {
	Event::addEventListener([](LeftEV ev) {
		string logfile = getfilename(login);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< "Left"
			<< ev.Player->getNameTag()
			<< ev.Player->getDimensionId()
			<< "xuid"
			<< ev.xuid;
		addLog(logfile, csv.getstr());
	});
}

void reg() {
	if (LogJoin)
		Join();
	if (LogLeft)
		Left();
}
void BLog_entery() {
	cout
		<< " ____       _                 _            _" << endl
		<< "|  _ \\     | |               (_)          | |" << endl
		<< "| |_) | ___| |__   __ ___   ___  ___  _ __| |     ___   __ _" << endl
		<< "|  _ < / _ \\ '_ \\ / _` \\ \\ / / |/ _ \\| '__| |    / _ \\ / _` |" << endl
		<< "| |_) |  __/ | | | (_| |\\ V /| | (_) | |  | |___| (_) | (_| |" << endl
		<< "|____/ \\___|_| |_|\\__,_| \\_/ |_|\\___/|_|  |______\\___/ \\__, |" << endl
		<< "                                                        __/ |" << endl
		<< "                                                       |___/" << endl
		<< "[LLBehaviorLog] BehaviorForLiteLoader Loaded By WangYneos,DreamGuXiang"
		<< "\n[LLBehaviorLog] " << "Target: 1.16.210.06   Build Date : " << __TIMESTAMP__ << endl;
	std::filesystem::create_directory("log");

#ifdef IsDebugVer
	cout << "[Warn] DebugVersion !!!!!!!!!!!!!!!" << endl;
#endif
	thread logt(sync_logger);
	logt.detach();
	thread listener_thread(reg);
	listener_thread.detach();
	loadconf();
	thread logger(sync_logger);
	logger.detach();
}