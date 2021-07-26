#include "pch.h"

namespace SymBlock {
	inline BlockLegacy* getLegacyBlock(Block* block) {//Block::getDebugText
		return dAccess<BlockLegacy*, 16>(block);
	}
	inline float getExplosionResistance(BlockLegacy* _this) {
		return dAccess<float, 76>(_this);
	}
}
namespace SymBlockSource {
	inline Block* getBlock(BlockSource* bs, BlockPos* bpos) {
		return SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", Block*, BlockSource*, BlockPos*)(bs, bpos);
	}
	inline int getDimensionId(BlockSource* bs) {
		auto dim = dAccess<Dimension*, 32>(bs);
		return dAccess<int, 216>(dim);
	}
	inline BlockActor* getBlockActor(BlockSource* a1, BlockPos* a2) {
		return SymCall("?getBlockEntity@BlockSource@@QEAAPEAVBlockActor@@AEBVBlockPos@@@Z", 
			BlockActor * ,BlockSource*, BlockPos*)(a1, a2);
	}
	inline BlockActor* getBlockActorByInt(BlockSource* a1, int a2, __int16 a3, int a4) {
		return SymCall("?getBlockEntity@BlockSource@@QEAAPEAVBlockActor@@HHH@Z",
			BlockActor*, BlockSource*, int,__int16,int)(a1, a2,a3,a4);
	}
}
namespace offActor {
	inline BlockSource* getBlockSource(Actor* ac) {
		return dAccess<BlockSource*, 872>(ac);
	}
}
#include <api/serviceLocate.h>
namespace symlevel {
	inline void save() {
		return SymCall("?save@Level@@UEAAXXZ", void, Level*)(LocateService<Level>());
	}
}
struct PointSelector {
	int mode;
	int dim;
	bool selectedA;
	BlockPos A;
	std::tuple<int, int,int> pos() {
		return { A.x,A.y,A.z };
	}
	bool click(BlockPos pos, int _dim) {
		if (mode == 1) { //A
			selectedA = true;
			A = pos;
			dim = _dim;
			return true;
		}
	}
	bool selected() {
		return selectedA;
	}
};

