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
THook(bool, "?canDestroy@WitherBoss@@SA_NAEBVBlock@@@Z", void* blockpos){
	if (NO_EXPLOSION) {
		return false;
	}
	return original(blockpos);
}

THook(bool, "?canDestroyBlock@WitherSkull@@UEBA_NAEBVBlock@@@Z", void* thi, void* blockpos){
if (NO_EXPLOSION) {
	return false;
}
return original(thi, blockpos);
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
		wp.sendText(u8"¡ìl¡ì6[¡ìeMCBE ¡ìbFINE¡ì6]¡ìr ËÀÍö³Í·££¬¿Û³ý50Ã¨Á¸");
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
	cout << u8"·¢ÏÖPurchaseReceiptPacket¹¥»÷£¬ÒÑÀ¹½Ø" << endl;
	return 0;
}
