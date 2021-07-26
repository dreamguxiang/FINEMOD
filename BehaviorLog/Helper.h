#include <loader\Loader.h>
#include <mc/BlockSource.h>
#include <mc/Block.h>
#include <string>
//offset Based on 1.16.220

using namespace std;
namespace SymCommandUtils {
	string getActorName(Actor* mob) {
		return SymCall("?getActorName@CommandUtils@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z", string, Actor*)(mob);
	}
}
namespace SymBlock {
	inline BlockLegacy* getLegacyBlock(Block* block) {//Block::getDebugText
		return dAccess<BlockLegacy*, 16>(block);
	}
}
namespace SymActorDamageSource {
	inline string lookupCauseName(unsigned int code) {
		string* cause_n = SymCall("?lookupCauseName@ActorDamageSource@@SAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorDamageCause@@@Z", string*, unsigned int)(code);
		return *cause_n;
	}
}
namespace SymBlockSource {
	inline Block* getBlock(BlockSource* bs, BlockPos* bpos) {
		return SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", Block*, BlockSource*, BlockPos*)(bs, bpos);
	}
	inline Block* getBlockint(BlockSource* bs, int x,int y, int z) {
		return SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@HHH@Z", Block*, BlockSource*, int,int,int)(bs, x,y,z);
	}
}
namespace offBlockLegacy {
	inline auto getBlockItemId(BlockLegacy* a1) {
		short v3 = *(short*)((__int64)a1 + 164);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
	string getFullName(BlockLegacy* Bl) {
		return  *(std::string*)((__int64)Bl + 128);
	}
}
namespace offBlock {
	string getFullName(Block* Bl) {
		return  *(string*)((void*)(SymBlock::getLegacyBlock(Bl) + 128));
	}
	inline BlockSource* getBlockSource(Actor* ac) {
		return dAccess<BlockSource*, 872>(ac);
	}
	inline Level* getLevel(Actor* pl) {
		// return SymCall("?getLevel@Actor@@QEBAAEBVLevel@@XZ", Level*, Actor*)(pl);
		return (Level*)*((uintptr_t*)((uintptr_t)pl + 888));
	}
}

namespace offGameMode {
	inline Player* getPlayer(void* Gamemode) {
		return dAccess<Player*, 8>(Gamemode);
	}
}
namespace offCommandBlockUpdatePacket {
	inline BlockPos getBlockPos(void* CmdBlockUpdatePk) {
		return dAccess<BlockPos, 48>(CmdBlockUpdatePk);
	}
	inline string getCommand(void* CmdBlockUpdatePk) {
		return dAccess<string, 72>(CmdBlockUpdatePk);
	}
	inline bool isBlock(void* CmdBlockUpdatePk) {
		return (bool)dAccess<std::byte, 174>(CmdBlockUpdatePk);
	}
}
namespace offLevelContainerModel {
	inline BlockPos* getBlockPos(void* LCM) {
		return (BlockPos*)((char*)LCM + 216);
	}
	inline Player* getPlayer(void* LCM) {
		return ((Player**)LCM)[26];
	}
}
namespace offItemStack {
	inline int getCount(ItemStack* a1) {
		return *((char*)a1 + 34);
		//LevelContainerModel::_getContainer
	}
}