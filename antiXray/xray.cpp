#include "pch.h"
#include < unordered_map >
#include<mc/OffsetHelper.h>
#include <api/scheduler/scheduler.h>
using namespace std;
constexpr unsigned int H(string_view s)
{
	auto str = s.data();
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) + (*str++); 
	}
	hash &= ~(1 << 31);
	return hash;
}

void Debug(string a1) {
	cout << a1 << endl;
	auto pls = liteloader::getAllPlayers();
	for (auto i : pls) {
		WPlayer(*i).sendText(a1);
	}
}


namespace config {
	int X = 80;
	int Y = 80;
	int high = 55;
}
bool isLiquid(Block* bl) {
	return SymCall("?isLiquidSource@BlockUtils@@SA_NAEBVBlock@@@Z", bool, Block*)(bl);
}

Block& Str2Block(string str) {
	string bstr = "?m" + str + "@VanillaBlocks@@3PEBVBlock@@EB";
	return **(Block**)dlsym_real(bstr.c_str());
}
#define setBlockSym "?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z"

void Strsetblock(string str, BlockSource* bs, BlockPos bp) {
	if (&Str2Block(str)) {
		SymCall(setBlockSym, bool, BlockSource*, BlockPos, Block&, int, __int64*)(bs, bp, Str2Block(str), 3, 0i64);
	}
}

void Replace(Block* block, BlockPos blockpos, BlockSource* bs) {
	switch (H(offBlock::getFullName(block))) {
	case H("minecraft:stone"): {
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			switch (H(offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))))) {
			case H("minecraft:deepslate_coal_ore"): 
				Strsetblock("DeepslateCoalOre", bs, blockpos);
				//Debug(u8"替换煤炭成功");
				break; 
			case H("minecraft:coal_ore"):
				Strsetblock("CoalOre", bs, blockpos);
				//Debug(u8"替换煤炭成功");
				break;
			case H("minecraft:diamond_ore"): 
				Strsetblock("DiamondOre", bs, blockpos);
				//Debug(u8"替换钻石成功");
				break; 
			case H("minecraft:deepslate_diamond_ore"):
				Strsetblock("DeepslateDiamondOre", bs, blockpos);
				//Debug(u8"替换钻石成功");
				break;
			case H("minecraft:emrald_ore"): 
				Strsetblock("EmeraldOre", bs, blockpos);
				//Debug(u8"替换绿宝石成功");
				break; 
			case H("minecraft:deepslate_emrald_ore"):
				Strsetblock("DeepslateEmeraldOre", bs, blockpos);
				//Debug(u8"替换深层绿宝石成功");
				break;
			case H("minecraft:gold_ore"): 
				Strsetblock("GoldOre", bs, blockpos);
				//Debug(u8"替换金成功");
				break; 
			case H("minecraft:deepslate_gold_ore"):
				Strsetblock("DeepslateGoldOre", bs, blockpos);
				//Debug(u8"替换深层金成功");
				break;
			case H("minecraft:redstone_ore"):
				Strsetblock("RedStoneOre", bs, blockpos);
				//Debug(u8"替换红石成功");
				break;
			case H("minecraft:deepslate_redstone_ore"):
				Strsetblock("DeepslateRedstoneOre", bs, blockpos);
				//Debug(u8"替换深层红石成功");
				break;
			case H("minecraft:iron_ore"):
				Strsetblock("IronOre", bs, blockpos);
				//Debug(u8"替换铁成功");
				break;
			case H("minecraft:deepslate_iron_ore"):
				Strsetblock("DeepslateIronOre", bs, blockpos);
				//Debug(u8"替换深层铁成功");
				break;
			case H("minecraft:lapis_ore"):
				Strsetblock("LapisOre", bs, blockpos);
				//Debug(u8"替换青金石成功");
				break;
			case H("minecraft:deepslate_lapis_ore"):
				Strsetblock("DeepslateLapisOre", bs, blockpos);
				//Debug(u8"替换深层青金石成功");
				break;
			case H("minecraft:copper_ore"):
				Strsetblock("CopperOre", bs, blockpos);
				//Debug(u8"替换铜成功");
				break;
			case H("minecraft:deepslate_copper_ore"):
				Strsetblock("DeepslateCopperOre", bs, blockpos);
				//Debug(u8"替换深层铜成功");
				break;
			}
		}
		break;
	}
	case H("minecraft:netherrack"): {
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			switch (H(offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))))) {
			case H("minecraft:quartz_ore"):
				Strsetblock("QuartzOre", bs, blockpos);
				//Debug(u8"替换下届石英成功");
				break;
			case H("minecraft:ancient_debris"):
				Strsetblock("AncientDebris", bs, blockpos);
				//Debug(u8"替换远古残骸成功");
				break;
			case H("minecraft:nether_gold_ore"):
				Strsetblock("NetherGoldOre", bs, blockpos);
				//Debug(u8"替换下届金成功");
				break;
			}
		}
		break;
	}
	case H("minecraft:coal_ore"): {
		//Debug(u8"监测到煤炭");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:coal_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_coal_ore"): {
		//Debug(u8"监测到深层煤炭");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_coal_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:diamond_ore"): {
		//Debug(u8"监测到钻石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:diamond_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_diamond_ore"): {
		//Debug(u8"监测到深层钻石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_diamond_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:emrald_ore"): {
		//Debug(u8"监测到绿宝石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:emrald_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_emrald_ore"): {
		//Debug(u8"监测到深层绿宝石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_emrald_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:gold_ore"): {
		//Debug(u8"监测到金");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:gold_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_gold_ore"): {
		//Debug(u8"监测到深层金");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_gold_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:redstone_ore"): {
		//Debug(u8"监测到红石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:redstone_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_redstone_ore"): {
		//Debug(u8"监测到深层红石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_redstone_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:iron_ore"): {
		//Debug(u8"监测到铁");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:iron_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_iron_ore"): {
		//Debug(u8"监测到深层铁");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_iron_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:lapis_ore"): {
		//Debug(u8"监测到青金石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:lapis_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_lapis_ore"): {
		//Debug(u8"监测到深层青金石");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_lapis_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:copper_ore"): {
		//Debug(u8"监测到铜");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:copper_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:deepslate_copper_ore"): {
		//Debug(u8"监测到深层铜");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:deepslate_copper_ore")Strsetblock("Stone", bs, blockpos);
		}
		break;
	}
	case H("minecraft:quartz_ore"): {
		//Debug(u8"监测到下届石英");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:quartz_ore")Strsetblock("Netherrack", bs, blockpos);
		}
		break;
	}
	case H("minecraft:ancient_debris"): {
		//Debug(u8"监测到远古残骸");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:ancient_debris")Strsetblock("Netherrack", bs, blockpos);
		}
		break;
	}
	case H("minecraft:nether_gold_ore"): {
		//Debug(u8"监测到下届金");
		if (offBlock::getFullName(&bs->getBlock(blockpos.add(1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(-1, 0, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(-1, 0, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, -1, 0))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, -1, 0))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, 1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, 1))) &&
			offBlock::getFullName(&bs->getBlock(blockpos.add(0, 0, -1))) != "minecraft:air" && !isLiquid(&bs->getBlock(blockpos.add(0, 0, -1)))) {
			//Debug(u8"对面的方块为" + offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))));
			if (offBlock::getFullName(&bs->getBlock(blockpos.add(config::X, 0, config::Y))) != "minecraft:nether_gold_ore")Strsetblock("Netherrack", bs, blockpos);
		}
		break;
	}
	}
}
void sche() {
	Handler::schedule(RepeatingTask([] {
		auto allplayer = liteloader::getAllPlayers();
		for (auto p : allplayer) {
			auto plpos = p->getPos();
			if (plpos.y < config::high) {
				for (int i = 2; i <= 10; i++) {
					for (int j = 2; j <= 10; j++) {
						for (int k = 2; k <= 10; k++) {
							auto oldpos = plpos.toBlockPos().add(-5, -5, -5).add(i, j, k);
							if (offBlock::getFullName(&offPlayer::getBlockSource(p)->getBlock(plpos.toBlockPos().add(-5, -5, -5).add(i, j, k))) != "minecraft:air");
							Replace(&offPlayer::getBlockSource(p)->getBlock(plpos.toBlockPos().add(-5, -5, -5).add(i, j, k)), plpos.toBlockPos().add(-5, -5, -5).add(i, j, k), offPlayer::getBlockSource(p));
						}
					}
				}
			}
		}
	}, 1));
}
void entry() {
	cout
		<< " _____ __   __  ___        _   ___   __                " << endl
		<< "|  __ \\\\ \\ / / / _ \\      | | (_) \\ / /                " << endl
		<< "| |  \\/ \\ V / / /_\\ \\_ __ | |_ _ \\ V / _ __ __ _ _   _ " << endl
		<< "| | __  /   \\ |  _  | '_ \\| __| |/   \\| '__/ _` | | | |" << endl
		<< "| |_\\ \\/ /^\\ \\| | | | | | | |_| / /^\\ \\ | | (_| | |_| |" << endl
		<< " \\____/\\/   \\/\\_| |_/_| |_|\\__|_\\/   \\/_|  \\__,_|\\__, |" << endl
		<< "                                                  __/ |" << endl
		<< "                                                 |___/ " << endl
		<< "[GXAntiXray] AntiXrayForLiteLoader Loaded By DreamGuXiang"
		<< "\n[GXAntiXray] " << "Target: 1.17.1.01   Build Date : " << __TIMESTAMP__ << endl;
	Event::addEventListener([](ServerStartedEV) {
		sche();
		});
}


THook(bool, "?playerWillDestroy@BlockLegacy@@UEBA_NAEAVPlayer@@AEBVBlockPos@@AEBVBlock@@@Z",
	BlockLegacy* _this, Player* pl, BlockPos blkpos, Block* bl) {
	auto plpos = pl->getPos();
	if (plpos.y < config::high) {
		switch (H(offBlock::getFullName(bl))) {
		case H("minecraft:redstone_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:redstone_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_redstone_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_redstone_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:lit_redstone_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:lit_redstone_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_lit_redstone_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_lit_redstone_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:minecraft:lapis_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:lapis_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:minecraft:deepslate_lapis_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_lapis_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:iron_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:iron_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_iron_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_iron_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:gold_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:gold_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_gold_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_gold_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:emrald_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:emrald_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_emrald_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_emrald_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:diamond_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:diamond_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_diamond_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_diamond_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:coal_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:coal_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_coal_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_coal_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:copper_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:copper_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:deepslate_copper_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:deepslate_copper_ore") {
				Strsetblock("Stone", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:quartz_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:quartz_ore") {
				Strsetblock("Netherrack", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:ancient_debris"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:ancient_debris") {
				Strsetblock("Netherrack", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		case H("minecraft:nether_gold_ore"): {
			if (offBlock::getFullName(&offPlayer::getBlockSource(pl)->getBlock(blkpos.add(config::X, 0, config::Y))) == "minecraft:nether_gold_ore") {
				Strsetblock("Netherrack", offPlayer::getBlockSource(pl), blkpos.add(config::X, 0, config::Y));
				//Debug(u8"破环设置石头");
			}
			break;
		}
		}
	}
	return original(_this, pl, blkpos, bl);
}