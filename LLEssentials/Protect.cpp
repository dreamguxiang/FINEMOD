#include "pch.h"
#include "Helper.h"
#include <llmoney.h>

THook(void, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* a2, Actor* a3, Vec3* a4, float a5, bool a6, bool a7, float a8, bool a9) {
	if (NO_EXPLOSION) {
		auto dimid = dAccess<int, 216>(dAccess<void*, 32>(a2));
		if (dimid == 0) {
			return original(_this, a2, a3, a4, a5, a6, 0, a8, a9);
		}
	}
	return original(_this, a2, a3, a4,a5,a6,a7,a8,a9);
}

THook(void, "?explode@RespawnAnchorBlock@@CAXAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* a1, BlockPos* a2, BlockSource* a3, Level* a4) {
	if (NO_EXPLOSION) {
		return;
	}
	return original(a1,a2,a3,a4);
}
THook(bool, "?canDestroy@WitherBoss@@SA_NAEBVBlock@@@Z", void* block){
	throw 1;
}
THook(void, "?_destroyBlocks@WitherBoss@@AEAAXAEAVLevel@@AEBVAABB@@AEAVBlockSource@@H@Z", void* a1, Level* a2, AABB* a3, BlockSource* a4, int a5) {

}


THook(bool, "?canDestroyBlock@WitherSkull@@UEBA_NAEBVBlock@@@Z", void* thi, void* block){
if (NO_EXPLOSION) {
	return false;
}
return original(thi, block);
}


THook(void*, "?write@StartGamePacket@@UEBAXAEAVBinaryStream@@@Z", void* a, void* b) {
	if (FAKE_SEED) {
		dAccess<int, 48>(a) = FAKE_SEED;
	}
	return original(a, b);
}

THook(void, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z"
	, void* __this, void* a2, void* a3, void* a4, float a5) {
	if (pfarm) {
		return;
	}
	return original(__this, a2, a3, a4, a5);
}

THook(bool, "?canUse@EndermanTakeBlockGoal@@UEAA_NXZ",
	void* __this) {
	if (penderman) {
		return false;
	}
	return original(__this);
}

THook(void*, "?write@ResourcePacksInfoPacket@@UEBAXAEAVBinaryStream@@@Z", void* a, void* b) {
	if (Force_ResourcePacks) {
		dAccess<bool, 48>(a) = true;
	}
	return original(a, b);
}
THook(void, "?registerCommand@CommandRegistry@@QEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$a"
	"llocator@D@2@@std@@PEBDW4CommandPermissionLevel@@UCommandFlag@@3@Z", 
	void* a1, void* a2, void* a3, char a4, char a5, char a6) {
	if (nocheatcmd) {
		return original(a1, a2, a3, a4, a5 |= 0x40, a6);
	}
	return original(a1, a2, a3, a4, a5, a6);
}

THook(void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z", Mob* _this, ActorDamageSource* a2) 
 {
	auto sp = MakeSP(_this);
	if (sp) {
		WPlayer wp = WPlayer{ *sp };
		Money::reduceMoney(offPlayer::getXUID(wp), 50);
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r 死亡惩罚，扣除50猫粮");
	}
	return original(_this,a2);
}

THook(bool, "?baseUseItem@GameMode@@QEAA_NAEAVItemStack@@@Z", void* a1, ItemStack* a2) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(a1) + 8);
	if (!a2->isNull()) {
		string  mstr2 = "";
		SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(a2->getItem(), &mstr2);
		if (mstr2 == "minecraft:clock") {
			liteloader::runcmdAs(sp, "cd");
		}
	}
	return original(a1, a2);
}

THook(bool, "?_attachedBlockWalker@PistonBlockActor@@AEAA_NAEAVBlockSource@@AEBVBlockPos@@EE@Z"
	, void* _this, BlockSource* bs, BlockPos bp, unsigned __int8 a4, unsigned __int8 a5) {
	auto bl = &bs->getBlock(bp);
	if (offBlock::getFullName(&bs->getBlock(bp)) == "minecraft:dragon_egg") {
		return false;
	}
	return original(_this, bs, bp, a4, a5);
}

THook(void, "?_teleport@DragonEggBlock@@AEBAXAEAVBlockSource@@AEAVRandom@@AEBVBlockPos@@@Z", void* a1, struct BlockSource* a2, struct Random* a3, struct BlockPos* a4) {
	return;
}


THook(void, "?_Throw_bad_optional_access@std@@YAXXZ") {
	throw 1;
}

THook(void, "?getEntities@LevelChunk@@QEBAXV?$span@V?$not_null@PEBVActor@@@gsl@@$0?0@gsl@@AEBVAABB@@AEAV?$vector@PEAVActor@@V?$allocator@PEAVActor@@@std@@@std@@_N@Z",
	__int64 a1, char* a2, const struct AABB* a3, __int64 a4, char a5) {
	try {
		original(a1, a2, a3, a4, a5);
	}
	catch (...) {
		return;
	}
}

THook(__int64, "?_read@PurchaseReceiptPacket@@EEAA?AW4StreamReadResult@@AEAVReadOnlyBinaryStream@@@Z", char* a1, void* a2) {
	cout << u8"发现PurchaseReceiptPacket攻击，已拦截" << endl;
	return 0;
}
class NetworkHandler;
namespace nick {
	string dimname(int a1) {
		switch (a1) {
		case 0:
			return u8"§a主世界";
		case 1:
			return u8"§c地狱";
		case 2:
			return u8"§g末地";
		default:
			return u8"§4神必世界";
		}
	}
	string getDeviceName(int a1) {
		switch (a1) {
		case -1:
			return u8"§2unkown";
		case 1:
			return u8"§b安卓";
		case 2:
			return u8"§diOS";
		case 3:
			return u8"§eOSX";
		case 4:
			return u8"§eAmazon";
		case 5:
			return u8"§eGearVR";
		case 6:
			return u8"§eHololens";
		case 7:
			return u8"§eWin10";
		case 8:
			return u8"§eWIN32";
		case 9:
			return u8"§eDedicated";
		case 10:
			return u8"§eTVOS";
		case 11:
			return u8"§ePlayStation";
		case 12:
			return u8"§eNintendo";
		case 13:
			return u8"§eXbox";
		case 14:
			return u8"§eWwindowsPhone";
		default:
			return u8"§2unkown";
		}
	}
	string getadmin(ServerPlayer* sp) {
		if (sp->getCommandPermissionLevel() > 0) {
			return u8"§4管理员";
		}
		return u8"§e玩家";
	}
	class NetworkPeer {
	public:
		enum class Reliability : int {};
		enum class DataStatus : int { OK, BUSY };
		struct NetworkStatus {
			int level;
			int ping, avgping;
			double packetloss, avgpacketloss;
		};

		virtual ~NetworkPeer();
		virtual void sendPacket(std::string, NetworkPeer::Reliability, int) = 0;
		virtual DataStatus receivePacket(std::string&) = 0;
		virtual NetworkStatus getNetworkStatus() = 0;
		MCAPI virtual void update();
		MCAPI virtual void flush(std::function<void(void)>&&);
	};
	string getstrms(int ms) {
		if (ms <= 30) {
			return u8"§a" + to_string(ms) + "ms";
		}
		else if (ms <= 50) {
			return  u8"§2" + to_string(ms) + "ms";
		}
		else if (ms <= 100) {
			return u8"§g" + to_string(ms) + "ms";
		}
		else {
			return u8"§c" + to_string(ms) + "ms";
		}
		return u8"" + to_string(ms) + "ms";
	}
	string getstrloss(double loss) {
		int a = loss * 100;
		return to_string(a);
	}
	string getms(ServerPlayer* sp) {
		auto netid = offPlayer::getNetworkIdentifier(sp);
		auto peer = SymCall("?getPeerForUser@NetworkHandler@@QEAAPEAVNetworkPeer@@AEBVNetworkIdentifier@@@Z", NetworkPeer*, NetworkHandler*, NetworkIdentifier*)(LocateService<Minecraft>()->getNetworkHandler(), netid);
		auto status = peer->getNetworkStatus();
		return	getstrms(status.avgping / 2);
	}
	string getpacketloss(ServerPlayer* sp) {
		auto netid = offPlayer::getNetworkIdentifier(sp);
		auto peer = SymCall("?getPeerForUser@NetworkHandler@@QEAAPEAVNetworkPeer@@AEBVNetworkIdentifier@@@Z", NetworkPeer*, NetworkHandler*, NetworkIdentifier*)(LocateService<Minecraft>()->getNetworkHandler(), netid);
		auto status = peer->getNetworkStatus();
		return	getstrloss(status.avgpacketloss);
	}
	string getchxt(ServerPlayer* pl,string name) {
		for (auto itergx = chxtdb.begin(); itergx != chxtdb.end(); itergx++) {
			if (name == itergx->first) {
				return itergx->second;
			}
		}
		return getadmin(pl);
	}
	string getcname(string name) {
		for (auto iteras = CNAME.begin(); iteras != CNAME.end(); iteras++) {
			if (name == iteras->first) {
				return iteras->second;
			}
		}
		return name;
	}
	void sendallplayer(ServerPlayer* sp, string msg) {
		for (auto pl : liteloader::getAllPlayers()) {
			auto iter = DeviceOS.find(offPlayer::getRealName(sp));
			if (iter != DeviceOS.end()) {
				string name = offPlayer::getRealName(sp);
				auto chs = getchxt(sp,name);
				auto cname = getcname(name);
				sendText((ServerPlayer*)pl, u8"§6[" + nick::dimname(sp->getDimensionId()) + u8"§6][" +
					nick::getDeviceName(DeviceOS[offPlayer::getRealName(sp)]) + u8"§6][" + getms(sp)+ u8"§6][§r" + chs + u8"§6]§r" + cname + u8"§r>" + msg, RAW);
			}
		}
	}
}

THook(void,"?_displayGameMessage@ServerNetworkHandler@@AEAAXAEBVPlayer@@AEAUChatEvent@@@Z",
	void* snh,ServerPlayer* sp,string* msg) {
	nick::sendallplayer(sp,*msg);
	return;
}

THook(bool, "?_couldHang@LanternBlock@@AEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z", void* a1, BlockSource* a2, BlockPos* a3) {
	return 1;
}
THook(bool, "?canSurvive@LanternBlock@@UEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z", void* a1, BlockSource* a2, BlockPos* a3) {
	return 1;
}

THook(bool, "?hasValidAttachment@BellBlock@@QEBA_NAEBVBlock@@AEAVBlockSource@@AEBVBlockPos@@@Z", void* a1, const struct Block* a2, struct BlockSource* a3, const struct BlockPos* a4) {
	return 1;
}
THook(bool, "?canAttachTo@ButtonBlock@@SA_NAEAVBlockSource@@AEBVBlockPos@@E@Z", struct BlockSource* a1, const struct BlockPos* a2, unsigned __int8 a3) {
	return 1;
}

THook(bool, "?canPushInItem@BrewingStandBlockActor@@UEBA_NAEAVBlockSource@@HHAEBVItemInstance@@@Z", void* a1, struct BlockSource* a2, int a3, int a4, const struct ItemInstance* a5) {
	return 1;
}


/*
THook(bool, "?shouldDespawn@Item@@QEBA_NXZ", Item* a1) {
	cout << (*((char*)a1 + 290) >> 6) << endl;
	return 0;
}

THook(Item*, "?setShouldDespawn@Item@@UEAAAEAV1@_N@Z", Item* a1, bool a2) {
	*((char*)a1 + 290) = 0;
	cout << *((char*)a1 + 290) << endl;
	return a1;
}*/
