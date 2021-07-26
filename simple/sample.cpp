
/*

*/  
#include "pch.h"
#include <map>
#include <regex>
#include <api/scheduler/scheduler.h>

constexpr unsigned int H(string_view s)
{
	auto str = s.data();
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) + (*str++); /* times 33 */
	}
	hash &= ~(1 << 31); /* strip the highest bit */
	return hash;
};

typedef unsigned long long eid_t;
static eid_t eid = 1099511628288;

enum TagType {
	End, Byte, Short, Int, Int64, Float, Double, ByteArray, String, List, Compound,
};

struct Tag
{
	char filler[40];
	template<typename T>
	T inline getVaule() {
		return *(T*)((uintptr_t)this + 8);
	}

	char inline getTagType() {
		return (*(__int64(__fastcall**)(const struct Tag*))(*(uintptr_t*)this + 40i64))(this);
	}
};

std::vector<std::string> getCustomLore(ItemStack* item) {
	Tag* tag = 0;
	SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		void*, void*, Tag**)((void*)item, &tag);
	std::vector<std::string> lore;
	if (tag->getTagType() == TagType::Compound) {
		auto nbt = tag->getVaule<std::map<string, Tag>>();
		if (nbt.find("tag") != nbt.end()) {
			auto tag = nbt["tag"].getVaule<std::map<string, Tag>>();
			if (tag.find("display") != tag.end()) {
				auto display = tag["display"].getVaule< std::map < string, Tag>>();
				if (display.find("Lore") != display.end()) {
					auto lores = display["Lore"].getVaule<std::vector<Tag*>>();
					for (auto i : lores) {
						auto l = i->getVaule<std::string>();
						lore.push_back(l);
					}
				}
			}
		}
	}
	return lore;
}

void sendPacket( int dimid,Vec3 pos,string Identifier) {
	/*
	WBStream bs;
	bs.apply(char(dimid), VarULong(ZigZag(pl->getUniqueID().id)), pos, MCString(Identifier)
	);
	MyPkt<0x76> pk(bs);
	((ServerPlayer*)pl)->sendNetworkPacket(pk);
	*/
	Packet* pkt;
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		void*, Packet**, int)(&pkt, 0x76);
	dAccess<char, 48>(pkt) = char(dimid);
	//dAccess<long long, 7>(pkt) = ZigZag(pl->getUniqueID().id);
	dAccess<float, 64>(pkt) = pos.x;
	dAccess<float, 68>(pkt) = pos.y;
	dAccess<float, 72>(pkt) = pos.z;
	dAccess<string, 80>(pkt) = Identifier;
	std::vector<Player*> plist = liteloader::getAllPlayers();
	for (auto p : plist) {
		auto sp = MakeSP(p);
		sp->sendNetworkPacket(*pkt);
	}
}

void sendsoundPacket(ServerPlayer* pl,  Vec3 pos, string Identifier) {
	/*
	WBStream bs;
	bs.apply(char(dimid), VarULong(ZigZag(pl->getUniqueID().id)), pos, MCString(Identifier)
	);
	MyPkt<0x76> pk(bs);
	((ServerPlayer*)pl)->sendNetworkPacket(pk);
	*/
	Packet* pkt;
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		void*, Packet**, int)(&pkt, 0x56);
	auto px = (int)pos.x;
	auto py = (int)pos.y;
	auto pz = (int)pos.z;
	if (px < 0)px = px - 1;
	if (pz < 0)pz = pz - 1;
	dAccess<string, 48>(pkt) = Identifier;
	dAccess<int, 80>(pkt) = px;
	dAccess<int, 84>(pkt) = py;
	dAccess<int, 88>(pkt) = pz;
	//dAccess<float, 92>(pkt) = 1;
	//dAccess<float, 96>(pkt) = 1;
	pl->sendNetworkPacket(*pkt);
}

#define N  999
bool possibility(float a1) {
	srand((unsigned)time(NULL) * 1000);
	float div = rand() % (N + 1) / (float)(N + 1);
	if (div < a1) {
		return true;
	}
	else {
		return false;
	}
}


THook(bool, "?attack@Player@@UEAA_NAEAVActor@@AEBW4ActorDamageCause@@@Z", Player* pl, Actor* a2, void* a3){
	auto item = pl->getSelectedItem();
	auto lore = getCustomLore(&item);
	auto sp = MakeSP(pl);
	srand((unsigned)time(NULL) * 1000);
	if (lore.size() == 0) {
		return original(pl, a2,a3);
	}

	switch (H(lore[0])) {
	case H(u8"寒冰"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		int ry = rand() % 2 + 1;
		float div = rand() % (N + 1) / (float)(N + 1);
		sendPacket(dimid, { px + div, py + ry ,pz + div }, "minecraft:water_wake_particle");
		sendPacket(dimid, { px - div, py + ry, pz - div }, "minecraft:water_wake_particle");
		sendPacket(dimid, { px + div, py + ry, pz - div }, "minecraft:water_wake_particle");
		break;
	}
	case H(u8"岩浆"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		int ry = rand() % (3 - 0 + 1) + 0;
		sendPacket(dimid, { px,py + ry,pz }, "lava_particle");
		sendPacket(dimid, { px,py + ry / 2,pz }, "lava_particle");
		sendPacket(dimid, { px,py + ry / 3,pz }, "lava_particle");
		sendPacket(dimid, { px,py + ry,pz }, "lava_particle");
		sendPacket(dimid, { px,py + ry / 2,pz }, "lava_particle");
		sendPacket(dimid, { px,py + ry / 3,pz }, "lava_particle");
		break;
	}
	case H(u8"音符"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		int ry = rand() % (3 - 0 + 1) + 0;
		sendPacket(dimid, { px,py + ry,pz }, "note");
		sendPacket(dimid, { px,py + ry / 2,pz }, "note");
		sendPacket(dimid, { px,py + ry / 3,pz }, "note");
		sendPacket(dimid, { px + ry / 2,py + ry,pz - ry / 2 }, "note");
		sendPacket(dimid, { px - ry / 2,py + ry / 2,pz + ry / 2 }, "note");
		sendPacket(dimid, { px + ry / 3,py + ry / 3,pz - ry / 2 }, "note");
		break;
	}
	case H(u8"血"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		sendPacket(dimid, { px, py + 1,pz }, "minecraft:falling_dust_dragon_egg_particle");
		sendPacket(dimid, { px, py + (float)0.5,pz }, "minecraft:falling_dust_dragon_egg_particle");
		sendPacket(dimid, { px, py + (float)1.2,pz }, "minecraft:falling_dust_dragon_egg_particle");
		if (possibility(0.35)) {
			sendPacket(dimid, { px,py + 1,pz }, "blood");
		}
		break;
	}
	case H(u8"末影"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		float ry = rand() % 2 + 1.5;
		sendPacket(dimid, { px,px + ry,pz }, "ender");
		sendPacket(dimid, { px,px + ry / 2,pz }, "ender");
		sendPacket(dimid, { px,px + ry / 3,pz }, "ender");
		sendPacket(dimid, { px,px + ry,pz }, "ender");
		sendPacket(dimid, { px,px + ry / 2,pz }, "ender");
		sendPacket(dimid, { px,px + ry / 3,pz }, "ender");
		break;
	}
	case H(u8"duang"): {
		if (possibility(0.3)) {
			auto dimid = a2->getDimensionId();
			auto playerPos = a2->getPos();
			auto Pos = sp->getPos();
			auto px = playerPos.x;
			auto py = playerPos.y;
			auto pz = playerPos.z;
			sendPacket(dimid, { px, py + (float)2.5,pz }, "minecraft:critical_hit_emitter");
			liteloader::runcmdEx("execute @a[name=\"" + offPlayer::getRealName(sp) + "\"] ~ ~ ~ playsound random.anvil_land @a " + std::to_string(Pos.x) + " " + std::to_string(Pos.y) + " " + std::to_string(Pos.z) + " 1 1");
		}

		break;
	}
	case H(u8"斩击"): {
		if (possibility(0.5)) {
			auto dimid = a2->getDimensionId();
			auto playerPos = a2->getPos();
			auto Pos = sp->getPos();
			auto px = playerPos.x;
			auto py = playerPos.y;
			auto pz = playerPos.z;
			sendPacket(dimid, { px,py + 2,pz }, "minecraft:dragon_destroy_block");
			sendPacket(dimid, { px,py + 1,pz }, "minecraft:falling_dust_dragon_egg_particle");
			sendPacket(dimid, { px,py + (float)0.5,pz }, "minecraft:falling_dust_dragon_egg_particle");
			sendPacket(dimid, { px,py + (float)1.2,pz }, "minecraft:falling_dust_dragon_egg_particle");
			sendPacket(dimid, { px,py + (float)0.2,pz }, "minecraft:falling_dust_dragon_egg_particle");
			sendPacket(dimid, { px,py + (float)1.5,pz }, "minecraft:falling_dust_dragon_egg_particle");
			liteloader::runcmdEx("execute @a[name=\"" + offPlayer::getRealName(sp) + "\"] ~ ~ ~ playsound item.trident.throw @a " + std::to_string(Pos.x) + " " + std::to_string(Pos.y) + " " + std::to_string(Pos.z) + " 1 1");
		}
		break;
	}
	case H(u8"七夕"): {
		auto dimid = a2->getDimensionId();
		auto playerPos = a2->getPos();
		auto px = playerPos.x;
		auto py = playerPos.y;
		auto pz = playerPos.z;
		int ry = rand() % (2 - 0 + 1) + 0;
		sendPacket(dimid, { px,py,pz }, "hearth");
		sendPacket(dimid, { px,py + 1,pz }, "hearth");
		sendPacket(dimid, { px,py + 2,pz }, "hearth");
		sendPacket(dimid, { px + ry,py + 1,pz + ry }, "hearth");
		sendPacket(dimid, { px - ry,py + 1,pz - ry }, "hearth");
		sendPacket(dimid, { px + ry,py + 1,pz - ry }, "hearth");
		break;
	}
	}
	return original(pl,a2,a3);
}
#include "Container.hpp"
void npccs() {
		std::vector<Player*> plist = liteloader::getAllPlayers();
		for (auto p : plist) {
			Container* a = dAccess<Container*, 1648>(p);
			auto b = a->getSlot();
			for (auto& item : b ) {
				auto lore = getCustomLore(item);
				srand((unsigned)time(NULL) * 1000);
				if (lore.size() == 0) {
					continue;
				}
				switch (H(lore[0])) {
				case H(u8"低语"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;
					sendPacket(dimid, { px,py + (float)0.6,pz }, "enchant-normal");
					sendPacket(dimid, { px + 1, py + (float)0.8,pz + 1 }, "enchant-normal");
					sendPacket(dimid, { px + (float)0.5,py + 1,pz - (float)0.5 }, "enchant-normal");
					sendPacket(dimid, { px - (float)0.5,py + 1,pz + (float)0.5 }, "enchant-normal");
					sendPacket(dimid, { px - 1,py + (float)0.8,pz - 1 }, "enchant-normal");
					sendPacket(dimid, { px,py + (float)0.5,pz }, "enchant-normal");
					float div = 0.5;
					sendPacket(dimid, { px + div,py + 1,pz + div }, "minecraft:enchanting_table_particle");
					sendPacket(dimid, { px - div,py + 1,pz - div }, "minecraft:enchanting_table_particle");
					sendPacket(dimid, { px + div,py + 1,pz - div }, "minecraft:enchanting_table_particle");
					sendPacket(dimid, { px - div,py + 1,pz - div }, "minecraft:enchanting_table_particle");
					break;
				}
				case H(u8"末影之心"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;
					int dif1 = 1, dif2 = 1;
					sendPacket(dimid, { px,py + 1,pz }, "ender");
					sendPacket(dimid, { px + dif1,py + 1,pz + dif1 }, "ender");
					sendPacket(dimid, { px - dif1,py + 1,pz - dif1 }, "ender");
					sendPacket(dimid, { px + dif1,py + 1,pz - dif1 }, "ender");
					sendPacket(dimid, { px - dif1,py + 1,pz - dif1 }, "ender");
					sendPacket(dimid, { px,py + 1,pz }, "ender");
					sendPacket(dimid, { px + dif2,py + (float)0.6,pz + dif2 }, "ender");
					sendPacket(dimid, { px - dif2,py + (float)0.6,pz - dif2 }, "ender");
					sendPacket(dimid, { px + dif2,py + (float)0.6,pz - dif2 }, "ender");
					sendPacket(dimid, { px - dif2,py + (float)0.6,pz - dif2 }, "ender");
					break;
				}
				case H(u8"萤火虫"): {
					if (possibility(0.25)) {
						auto dimid = p->getDimensionId();
						auto playerPos = p->getPos();
						auto px = playerPos.x;
						auto py = playerPos.y;
						auto pz = playerPos.z;
						sendPacket(dimid, { px,py + 1,pz }, "totem-normal");
					}
					break;
				}
				case H(u8"村民之友"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;
					srand((unsigned)time(NULL) * 1000);
					auto rd = -2 * rand() % (1 * 2 + 1) + 1 / 2;

					sendPacket(dimid, { px,py + 1,pz }, "villager-happy");
					sendPacket(dimid, { px,py + (float)0.2,pz }, "villager-happy");
					sendPacket(dimid, { px,py + (float)0.3,pz }, "villager-happy");
					sendPacket(dimid, { px,py + 2,pz }, "villager-happy");
					sendPacket(dimid, { px - rd,py + (float)2.2,pz + rd }, "villager-happy");
					sendPacket(dimid, { px + rd,py + (float)2.2,pz - rd }, "villager-happy");
					sendPacket(dimid, { px - rd,py + (float)2.2,pz - rd }, "villager-happy");
					sendPacket(dimid, { px + rd,py + (float)1.2,pz - rd }, "villager-happy");
					sendPacket(dimid, { px - rd,py + (float)1.2,pz - rd }, "villager-happy");
					sendPacket(dimid, { px + rd,py + (float)1.2,pz + rd }, "villager-happy");
					break;
				}
				case H(u8"村民之敌"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;

					sendPacket(dimid, { px,py + (float)2.5,pz }, "villager-angry");
					sendPacket(dimid, { px,py + 2,pz }, "lava_particle");
					sendPacket(dimid, { px,py + (float)2.5,pz }, "lava_particle");
					break;
				}
				case H(u8"着火"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;

					sendPacket(dimid, { px,py,pz }, "smoke");
					sendPacket(dimid, { px,py - (float)0.1,pz }, "lava_particle");
					sendPacket(dimid, { px,py,pz }, "lava_particle");
					break;
				}
				case H(u8"屠龙勇士"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;

					sendPacket(dimid, { px,py,pz }, "minecraft:conduit_attack_emitter");
					sendPacket(dimid, { px,py + (float)1.1,pz }, "minecraft:conduit_attack_emitter");
					sendPacket(dimid, { px,py + (float)1.2,pz }, "minecraft:conduit_attack_emittery");
					sendPacket(dimid, { px,py + (float)1.3,pz }, "minecraft:conduit_attack_emitter");
					sendPacket(dimid, { px,py + (float)1.4,pz }, "minecraft:conduit_attack_emitter");
					sendPacket(dimid, { px,py + (float)1.5,pz }, "minecraft:conduit_attack_emitter");
					srand((unsigned)time(NULL) * 1000);
					float div = rand() % (N + 1) / (float)(N + 1);
					sendPacket(dimid, { px,py + (float)1.5,pz }, "minecraft:basic_portal_particle");
					sendPacket(dimid, { px + div,py + (float)1.5,pz + div }, "minecraft:basic_portal_particle");
					sendPacket(dimid, { px - div,py + (float)1.5,pz - div }, "minecraft:basic_portal_particle");
					sendPacket(dimid, { px + div,py + (float)1.5,pz - div }, "minecraft:basic_portal_particle");
					sendPacket(dimid, { px - div,py + (float)1.5,pz + div }, "minecraft:basic_portal_particle");
					if (possibility(0.1)) {
						sendPacket( dimid, { px,py + (float)0.25,pz },"minecraft:splash_spell_emitter");
					}
					break;
				}
				case H(u8"花瓣"): {
					auto dimid = p->getDimensionId();
					auto playerPos = p->getPos();
					auto px = playerPos.x;
					auto py = playerPos.y;
					auto pz = playerPos.z;
					if (possibility(0.15)) {
						sendPacket(dimid, {px,py - (float)3.5,pz }, "spiral-pink-1");
					}
					break;
				}
				}
			}
		}
}

void entry() {
	Handler::schedule(RepeatingTask([] {
	npccs();
		}, 2));
}

class Logger2 {
	std::string path;
	std::ofstream logfile;
public:
	Logger2(std::string path) {
		this->path = path;
		logfile.open(path, std::ios::app);
	}
	template <typename T>
	Logger2& operator<< (T msg) {
		logfile << msg;
		return *this;
	}
};
Logger2 LOG1("./ItemList.txt");
using namespace std;
THook(void, "?registerItem@ItemRegistry@@SAXV?$SharedPtr@VItem@@@@@Z",
	__int64 a1) {
	auto item = (Item***)a1;
	string mstr2;
	SymCall("?getSerializedName@Item@@QEBA?AV?$basic_strin"
		"g@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item*, string*)(**item, &mstr2);
	auto id = *((unsigned __int16*)**item + 61);
	LOG1 << u8"NAME: " << mstr2 << " | ID: " << id << "\n";
	return original(a1);

}