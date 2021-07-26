#include "pch.h"
#include "Helper.h"
#include <api\scheduler\scheduler.h>
#include "commad.h"
short ticks;
map<string, short > mcc;
std::map < string, short > ::reverse_iterator iter;

THook(unsigned int, "?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z",
	MinecraftCommands* self, __int64 a2, std::shared_ptr<CommandContext> cmd, char a4) {
	auto ptr = cmd.get();
	CommandOrigin& cmdo = ptr->getOrigin();
	auto sp = MakeSP(cmdo);
	if (sp) {
		LOG1 << "[" << gettime() << u8" INFO][BH] " << offPlayer::getRealName(sp) << " CMD " << ptr->getCmd() << endl;
	}
	return original(self, a2, cmd, a4);
}

THook(void*, "?_onPlayerLeft@ServerNetworkHandler@@AEAAXPEAVServerPlayer@@_N@Z",
	void* _this, ServerPlayer* a2, bool a3) {
	auto a = original(_this, a2, a3);
	auto pln = offPlayer::getRealName(a2);
	std::map<string, short>::iterator key = mcc.find(pln);
	if (key != mcc.end())
	{
		mcc.erase(key);
	}
	auto playerPos = a2->getPos();
	auto px = (int)playerPos.x;
	auto py = (int)playerPos.y;
	auto pz = (int)playerPos.z;
	if (px < 0)px = px - 1;
	if (pz < 0)pz = pz - 1;
	auto dim = a2->getDimensionId();
	LOG1 << "[" << gettime() << u8" INFO][BH] " << pln << " left server  Pos:(" << px <<","<< py <<"," << pz <<","<<dim << ") xuid: " << offPlayer::getXUIDString(a2) <<endl;
	return a;
}

THook(void*, "?onPlayerJoined@ServerScoreboard@@UEAAXAEBVPlayer@@@Z",
	void* _this, Player* a2) {
	auto n = offPlayer::getNetworkIdentifier(a2);
	jzqz[offPlayer::getRealName(a2)] = 114514;
	if (auto it = CNAME.find(offPlayer::getRealName(a2)); it != CNAME.end()) {
		optional<WPlayer> aa = WPlayer(*(ServerPlayer*)a2);
		ORIG_NAME[aa.val().v] = offPlayer::getRealName(a2);
	}
	LOG1 << "[" << gettime() << u8" INFO][BH] " << offPlayer::getRealName(a2) << " joined server IP: " << liteloader::getIP(*n) << " xuid: "<< offPlayer::getXUIDString(a2) <<endl;
	return original(_this, a2);
}

struct  tmp
{
	char filler[48];
};

static string api[] = {
	u8"确认付款",
	u8"退出"
};
#include <llmoney.h>
map<string, long > mcs;
std::map < string, long > ::reverse_iterator iterd;
THook(bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* self, ItemStack* item, BlockPos* bpos, uchar a1, Vec3* plpos, Block* block) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(self) + 8);
	long a = getTimeStamp();
	auto playername = offPlayer::getRealName(sp);
	for (iterd = mcs.rbegin(); iterd != mcs.rend(); iterd++)
		if (iterd->first == playername) {
			if (a - mcs[playername] <= (long)50)
				return false;
		}
	mcs[playername] = a;
	mcs.insert(std::map < string, long > ::value_type(playername, a));
	auto Bl = dAccess<BlockLegacy*, 16>(block);
	auto blockname = *(string*)((void*)(Bl + 128));
		auto itemid = item->getId();
		if (logItems.count(itemid)) {
			string  mstr2 = "";
			SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(item->getItem(), &mstr2);
			LOG1 << "[" << gettime() << u8" INFO][ItemLog] "
				<< offPlayer::getRealName(sp)
				<< " used logitem(" << mstr2 << ")"
				<< " on " << blockname
				<< "(" << bpos->toString() << ")"
				<< endl;
		}
		if (banItems.count(itemid)) {
			forceKick(sp, "use ban item");
		}
	return original(self, item, bpos, a1, plpos, block);
}

THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	void* self, NetworkIdentifier* id, void* text) {
	auto pl = SymCall("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
		Player*, void*, void*, char)(self, id, *(char*)((uintptr_t)text + 16));
	auto msg = std::string(*(std::string*)((uintptr_t)text + 88));

	if (msg.length() >= MAX_CHAT_LEN)
		return;
	LOG1 << "[" << gettime() << u8" INFO][BH] <" << offPlayer::getRealName(pl) << "> " << msg << endl;
	return original(self, id, text);
}

THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVDisconnectPacket@@@Z",
	ServerNetworkHandler* _this, NetworkIdentifier* a2, void* Packet) {
	auto pl = SymCall("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
		Player*, void*, void*, char)(_this, a2, *(char*)((uintptr_t)Packet + 16));
	if (!pl) {
		LOG1 << "[" << gettime() << u8" INFO][BH] IP: " << liteloader::getIP(*a2) << u8" 尝试与服务器连接" << endl;
		return;
	}
	if (pl) {
		LOG1 << "[" << gettime() << u8" INFO][BH] " << offPlayer::getRealName(pl) << "  server IP: xuid: " << offPlayer::getXUIDString(pl) << endl;
	}
}


struct fmtReq {
	clock_t time;
	string A;
	int B;
	fmtReq() {}
	fmtReq(string_view b, clock_t d, int boo) : A(b), time(d), B(boo) {}
};

struct fmtSet {
	clock_t lastReq = 0;
	bool Accept = true;
};
static std::list<fmtReq> reqss;


bool DoCloseReq(decltype(reqss.begin()) rq, string res) {
	string playernamea = rq->A;
	vector<Player*> plist = liteloader::getAllPlayers();
	Player* fplA = nullptr;
	for (auto p : plist) {
		if (offPlayer::getRealName(p) == playernamea) {
			fplA = p;
			break;
		}
	}
	if (fplA) {
		WPlayer wp = { *fplA };
		if (res == "timeout") {
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§3主城附魔台付费超时");
			reqss.erase(rq);
		}
		if (res == "accept") {
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§a付款成功");
			reqss.erase(rq);
		}
		if (res == "nomoney") {
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§c你没有足够的猫粮");
			reqss.erase(rq);
		}
	}
	else {
		//no such player
	}
}

THook(unsigned short, "?onBlockInteractedWith@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlockPos@@@Z",
	void* _this, Player* sp, BlockPos* bp) {
	auto bs = offPlayer::getBlockSource(sp);
	auto block = SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", Block*, BlockSource*, BlockPos*)(bs, bp);
	auto Bl = dAccess<BlockLegacy*, 16>(block);
	auto blockname = *(string*)((void*)(Bl + 128));
	if (blockname == "minecraft:enchanting_table") {
		if (bp->toString() == "-157,66,106") {
			WPlayer wp = { *sp };
			auto playername = offPlayer::getRealName(sp);
			for (auto it = reqss.begin(); it != reqss.end(); ++it) {
				if (it->A == playername) {
					if (it->B == 1) {
						auto a = Money::getMoney(offPlayer::getXUID(sp));
						if (a > (long long)199) {
							Money::reduceMoney(offPlayer::getXUID(sp), 200);
							DoCloseReq(it, "accept");
							return original(_this, sp, bp);
						}
						else {
							DoCloseReq(it, "nomoney");
							return 0;
						}
					}
				}
			}
				fmtReq fmt;
				fmt.A = playername;
				fmt.B = 1;
				fmt.time = clock();
				reqss.push_back(fmt);
				wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§c你将花费200猫粮来使用一次主城附魔台\n§l§e付款后退出附魔台界面将要重新付款");
				for (auto it = reqss.begin(); it != reqss.end(); ++it) {
					if (it->A == playername) {
						if (it->B == 1) {
							return 0;
						}
					}
				}
			}
		}
	return original(_this, sp, bp);
}


#include <api/scheduler/scheduler.h>
void sschTask() {
	Handler::schedule(RepeatingTask([] {
		clock_t expire = clock() - 5000;
		for (auto it = reqss.begin(); it != reqss.end();) {
			if (it->time <= expire) {
				auto oldit = it;
				++it;
				DoCloseReq(oldit,"timeout");
			}
			else break;
		}
		}, 5)); //5sec
}
