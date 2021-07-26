#include "pch.h"
#include "ChestBlockActor.hpp"
#include "ChestBlock.hpp"
#include "lock.h"
#include <api/myPacket.h>
using namespace std;
std::unique_ptr<KVDBImpl> db;
typedef unsigned long long eid_t;
static eid_t CID = 9223372036854775809ull;

char* itoa1(int num, char* str, int radix)
{
	char index[] = "0123456789ABCDEF";
	unsigned unum;
	int i = 0, j, k;
	if (radix == 10 && num < 0)
	{
		unum = (unsigned)-num;
		str[i++] = '-';
	}
	else unum = (unsigned)num;
		do {
		str[i++] = index[unum % (unsigned)radix];
		unum /= radix;
	} while (unum);
	str[i] = '\0';

	if (str[0] == '-')
		k = 1;
	else
		k = 0;

	for (j = k; j <= (i - 1) / 2; j++)
	{
		char temp;
		temp = str[j];
		str[j] = str[i - 1 + k - j];
		str[i - 1 + k - j] = temp;
	}
	return str;
}
struct LockChest {
	BlockPos pos;
	int dim;
	string uniqid;
	string name;
	eid_t eid;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(eid, uniqid, pos,dim, name);
	}
	static unsigned long long getFLAGS(string_view val) {
		split_view spv(val);
		unsigned long long rv = 0;
		for (; !spv.end(); ++spv) {
			int bit = atoi(spv.get());
			rv |= 1ull << bit;
		}
		return rv;
	}
	void unpack(RBStream& bs) {
		bs.apply(eid, uniqid, pos, dim, name);
		string val;
	}
	static string getPosName(int a1,int a2,int a3,int a4) {
		string val;
		char x[25];
		char y[25];
		char z[25];
		char dim[25];
		auto xs = itoa1(a1, x, 10);
		auto ys = itoa1(a2, y, 10);
		auto zs = itoa1(a3, z, 10);
		auto dims = itoa1(a4, dim, 10);
		std::string const& cc = std::string(xs) + std::string(ys)+ std::string(zs) + std::string(dims);
		return cc;
	}

};
enum COP_LIST :int {
	LIST = 1
};

enum COP_LOCK :int {
	LOCK = 1,
	UNLOCK = 2
};

std::unordered_map<eid_t, LockChest> chests;
static playerMap<PointSelector> SELECT_POINT;
static playerMap<PointSelector> SELECT_UNPOINT;
bool CCMD_LIST(CommandOrigin const& ori, CommandOutput& outp, MyEnum<COP_LIST>, optional<string>& name) {
	for (auto& i : chests) {
		if (name.set) {
			if (i.second.name == name.value()) {
				outp.addMessage("OwnerID:" + i.second.name + " uniqid: " + S(i.second.uniqid) + u8"§r Pos: " + i.second.pos.toString() + " Dim: " + S(i.second.dim));
				return true;
			}
		}
		else {
			if (strstr(i.second.name.c_str(), "_cmd") == NULL) {
				outp.addMessage("OwnerID: " + i.second.name + " uniqid: " + S(i.second.uniqid) + u8" §rPos: " + i.second.pos.toString() + " Dim: " + S(i.second.dim));
			}
		}
	}
	return true;
}
bool CCMD_LOCK(CommandOrigin const& ori, CommandOutput& outp, MyEnum<COP_LOCK> op) {
	switch (op.val)
	{
	case COP_LOCK::LOCK: {
		auto wp = MakeWP(ori).val();
		auto sp = MakeSP(ori);
		SELECT_POINT[sp].mode = 1;
		wp.sendText(u8"请选择你想要上锁的箱子");
		return true;
	}
	case COP_LOCK::UNLOCK: {
		auto wp = MakeWP(ori).val();
		auto sp = MakeSP(ori);
		SELECT_UNPOINT[sp].mode = 1;
		wp.sendText(u8"请选择你想要解锁的箱子");
		return true;
	}
	}
}
THook(__int64,
	"?onBlockPlacedByPlayer@VanillaServerGameplayEventListener@@UEAA?AW4EventResult@@AEAVPlayer@@AEBVBlock@@AEBVBlockPos@@_N@Z",
	void* _this, Player* pl, Block* blk,  BlockPos* blkpos, bool _bool) {
	auto bl = offBlock::getLegacyBlock(blk);
	auto blname = offBlockLegacy::getFullName(bl);
	if (blname == "minecraft:chest" || blname == "minecraft:trapped_chest" || blname == "minecraft:ender_chest") {
		auto bs = offActor::getBlockSource(pl);
		auto ba = SymBlockSource::getBlockActor(bs, blkpos);
		ba->setCustomName(offPlayer::getRealName(pl) + u8"的箱子(已上锁)");
		ba->setCustomNameSaved(true);
		Packet* pkt;
		SymCall("?_getUpdatePacket@ChestBlockActor@@MEAA?AV?$unique_ptr@VBlockActorDataPacket@@U?$default_de"
			"lete@VBlockActorDataPacket@@@std@@@std@@AEAVBlockSource@@@Z", void, ChestBlockActor*, Packet**)((ChestBlockActor*)ba, &pkt);
		auto sp = MakeSP(pl);
		sp->sendNetworkPacket(*pkt);
		symlevel::save();
	}
	return original(_this, pl, blk, blkpos, _bool);
}

THook(bool,"?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* bs, Block* blk, BlockPos* blkpos, unsigned char a4, Actor* ac, bool _bool) {
	auto pl = MakeSP(ac);
	if (pl) {
		auto bl = offBlock::getLegacyBlock(blk);
		auto blname = offBlockLegacy::getFullName(bl);
		if (blname == "minecraft:chest" || blname == "minecraft:trapped_chest" || blname == "minecraft:ender_chest") {
			auto ba = SymBlockSource::getBlockActor(bs, blkpos);
			//std::cout<< blkpos->x << " "<< blkpos->y <<" "<<blkpos->z << std::endl;
			string val;
			auto namezuo = LockChest::getPosName(blkpos->x + 1, blkpos->y, blkpos->z, pl->getDimensionId());
			if ((db->get(string("c_") + namezuo, val))) {
				auto bas = SymBlockSource::getBlockActorByInt(bs, blkpos->x + 1, blkpos->y, blkpos->z);
				auto bo = SymCall("?isLargeChest@ChestBlockActor@@QEBA_NXZ", bool, ChestBlockActor*)((ChestBlockActor*)bas);
				if (!bo) {
					for (auto itSS = chests.begin(); itSS != chests.end(); ++itSS) {
						if (itSS->second.uniqid == namezuo) {
							if (itSS->second.name != offPlayer::getRealName(pl)) {
								auto wp = WPlayer{ *pl };
								wp.sendText(u8"你没有权限合并此箱子", RAW);
								return false;
							}
						}
					}
					LockChest chest;
					chest.pos = *blkpos;
					chest.dim = pl->getDimensionId();
					auto uids = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z, pl->getDimensionId());
					chest.uniqid = uids;
					chest.eid = CID++;
					db->put("CID", to_view(CID));
					chest.name = offPlayer::getRealName(pl);
					WBStream ws;
					ws.apply(chest);
					auto wp = WPlayer{ *pl };
					db->put("c_" + uids, ws);
					chests.emplace(CID - 1, std::move(chest));
					db->put("cbig_" + namezuo, uids);
					wp.sendText(u8"已合并箱子", RAW);
					return original(bs, blk,  blkpos, a4,ac, _bool);
				}
			}
			auto nameyou = LockChest::getPosName(blkpos->x - 1, blkpos->y, blkpos->z, pl->getDimensionId());
			if ((db->get(string("c_") + nameyou, val))) {
				auto bas = SymBlockSource::getBlockActorByInt(bs, blkpos->x - 1, blkpos->y, blkpos->z);
				auto bo = SymCall("?isLargeChest@ChestBlockActor@@QEBA_NXZ", bool, ChestBlockActor*)((ChestBlockActor*)bas);
				if (!bo) {
					for (auto itSS = chests.begin(); itSS != chests.end(); ++itSS) {
						if (itSS->second.uniqid == nameyou) {
							if (itSS->second.name != offPlayer::getRealName(pl)) {
								auto wp = WPlayer{ *pl };
								wp.sendText(u8"你没有权限合并此箱子", RAW);
								return false;
							}
						}
					}
					LockChest chest;
					chest.pos = *blkpos;
					chest.dim = pl->getDimensionId();
					auto uids = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z, pl->getDimensionId());
					chest.uniqid = uids;
					chest.eid = CID++;
					db->put("CID", to_view(CID));
					chest.name = offPlayer::getRealName(pl);
					WBStream ws;
					ws.apply(chest);
					auto wp = WPlayer{ *pl };
					db->put("c_" + uids, ws);
					chests.emplace(CID - 1, std::move(chest));
					db->put("cbig_" + nameyou, uids);
					auto baa = (ChestBlockActor*)ba;
					wp.sendText(u8"已合并箱子", RAW);
					return original(bs, blk, blkpos, a4, ac, _bool);

				}
			}
			auto nameqian = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z + 1, pl->getDimensionId());
			if ((db->get(string("c_") + nameqian, val))) {
				auto bas = SymBlockSource::getBlockActorByInt(bs, blkpos->x, blkpos->y, blkpos->z + 1);
				auto bo = SymCall("?isLargeChest@ChestBlockActor@@QEBA_NXZ", bool, ChestBlockActor*)((ChestBlockActor*)bas);
				if (!bo) {
					for (auto itSS = chests.begin(); itSS != chests.end(); ++itSS) {
						if (itSS->second.uniqid == nameqian) {
							if (itSS->second.name != offPlayer::getRealName(pl)) {
								auto wp = WPlayer{ *pl };
								wp.sendText(u8"你没有权限合并此箱子", RAW);
								return false;
							}
						}
					}
					LockChest chest;
					chest.pos = *blkpos;
					chest.dim = pl->getDimensionId();
					auto uids = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z, pl->getDimensionId());
					chest.uniqid = uids;
					chest.eid = CID++;
					db->put("CID", to_view(CID));
					chest.name = offPlayer::getRealName(pl);
					WBStream ws;
					ws.apply(chest);
					auto wp = WPlayer{ *pl };
					db->put("c_" + uids, ws);
					chests.emplace(CID - 1, std::move(chest));
					db->put("cbig_" + nameqian, uids);
					auto baa = (ChestBlockActor*)ba;
					wp.sendText(u8"已合并箱子", RAW);
					return original(bs, blk, blkpos, a4, ac, _bool);
				}
			}
			auto namehou = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z - 1, pl->getDimensionId());
			if ((db->get(string("c_") + namehou, val))) {
				auto bas = SymBlockSource::getBlockActorByInt(bs, blkpos->x, blkpos->y, blkpos->z - 1);
				auto abol = SymCall("?canPairWith@ChestBlockActor@@QEAA_NPEAVBlockActor@@AEAVBlockSource@@@Z",
					bool, ChestBlockActor*, BlockActor*, BlockSource*)((ChestBlockActor*)bas, ba, bs);
				if (abol) {
					auto bo = SymCall("?isLargeChest@ChestBlockActor@@QEBA_NXZ", bool, ChestBlockActor*)((ChestBlockActor*)bas);
					if (!bo) {
						for (auto itSS = chests.begin(); itSS != chests.end(); ++itSS) {
							if (itSS->second.uniqid == namehou) {
								if (itSS->second.name != offPlayer::getRealName(pl)) {
									auto wp = WPlayer{ *pl };
									wp.sendText(u8"你没有权限合并此箱子", RAW);
									return false;
								}
							}
						}
						LockChest chest;
						chest.pos = *blkpos;
						chest.dim = pl->getDimensionId();
						auto uids = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z, pl->getDimensionId());
						chest.uniqid = uids;
						chest.eid = CID++;
						db->put("CID", to_view(CID));
						chest.name = offPlayer::getRealName(pl);
						WBStream ws;
						ws.apply(chest);
						auto wp = WPlayer{ *pl };
						db->put("c_" + uids, ws);
						chests.emplace(CID - 1, std::move(chest));
						db->put("cbig_" + namehou, uids);
						auto baa = (ChestBlockActor*)ba;
						wp.sendText(u8"已合并箱子", RAW);
						return original(bs, blk, blkpos, a4, ac, _bool);
					}
				}
			}
			LockChest chest;
			chest.pos = *blkpos;
			chest.dim = pl->getDimensionId();
			auto uids = LockChest::getPosName(blkpos->x, blkpos->y, blkpos->z, pl->getDimensionId());
			chest.uniqid = uids;
			chest.eid = CID++;
			db->put("CID", to_view(CID));
			chest.name = offPlayer::getRealName(pl);
			WBStream ws;
			ws.apply(chest);
			auto wp = WPlayer{ *pl };
			db->put("c_" + uids, ws);
			wp.sendText(u8"已自动上锁", RAW);
			chests.emplace(CID - 1, std::move(chest));
		}
		return original(bs, blk, blkpos, a4, ac, _bool);
	}
}
/*
THook(void, "?_getUpdatePacket@ChestBlockActor@@MEAA?AV?$unique_ptr@VBlockActorDataPacket@"
	"@U?$default_delete@VBlockActorDataPacket@@@std@@@std@@AEAVBlockSource@@@Z", ChestBlockActor* a1, void* a2) {
	cout << a1->getCustomName() << endl;
	return original(a1, a2);
}
*/
THook(void, "?openBy@ChestBlockActor@@QEAAXAEAVPlayer@@@Z", 
	ChestBlockActor* a1, Player* a2) {
	auto pos = a1->getPosition();
	auto sp = (ServerPlayer*)a2;
	auto wp = WPlayer{ *sp };
	auto dim = a2->getDimensionId();
	auto name = LockChest::getPosName(pos.x, pos.y, pos.z, dim);
	auto it = SELECT_POINT._map.find(sp);
	if (it != SELECT_POINT._map.end()) {
		if (it->second.click(pos, dim)) {
			if (it->second.mode == 1) {
				string val;
				if ((db->get(string("c_") + name, val))) {
					wp.sendText(u8"这个箱子已经上锁", RAW);
					SELECT_POINT._map.erase(sp);
					return;
				}
				LockChest chest;
				chest.pos = pos;
				chest.dim = sp->getDimensionId();
				chest.uniqid = name;
				chest.eid = CID++;
				db->put("CID", to_view(CID));
				chest.name = offPlayer::getRealName(a2);
				WBStream ws;
				ws.apply(chest);
				a1->setCustomName(offPlayer::getRealName(sp) + u8"的箱子(已上锁)");
				a1->setCustomNameSaved(true);
				Packet* pkt;
				SymCall("?_getUpdatePacket@ChestBlockActor@@MEAA?AV?$unique_ptr@VBlockActorDataPacket@@U?$default_de"
					"lete@VBlockActorDataPacket@@@std@@@std@@AEAVBlockSource@@@Z", void, ChestBlockActor*, Packet**)((ChestBlockActor*)a1, &pkt);
				sp->sendNetworkPacket(*pkt);
				wp.sendText(u8"上锁成功", RAW);
				symlevel::save();
				db->put("c_" + name, ws);
				SELECT_POINT._map.erase(sp);
				chests.emplace(CID - 1, std::move(chest));
				return;
			}
		}
	}
	auto its = SELECT_UNPOINT._map.find(sp);
	if (its != SELECT_UNPOINT._map.end()){
		if (its->second.click(pos, dim)) {
			if (its->second.mode == 1) {
				string val;
				if (!(db->get(string("c_") + name, val))) {
					wp.sendText(u8"这个箱子没有被上锁", RAW);
					SELECT_UNPOINT._map.erase(sp);
					return;
				}
				for (auto itSS = chests.begin(); itSS != chests.end(); ++itSS) {
					if (itSS->second.uniqid == name) {
						if (itSS->second.name == offPlayer::getRealName(sp)) {
							a1->setCustomName(offPlayer::getRealName(sp) + u8"的箱子(未上锁)");
							a1->setCustomNameSaved(true);
							Packet* pkt;
							SymCall("?_getUpdatePacket@ChestBlockActor@@MEAA?AV?$unique_ptr@VBlockActorDataPacket@@U?$default_de"
								"lete@VBlockActorDataPacket@@@std@@@std@@AEAVBlockSource@@@Z", void, ChestBlockActor*, Packet**)((ChestBlockActor*)a1, &pkt);
							sp->sendNetworkPacket(*pkt);
							wp.sendText(u8"解锁成功", RAW);
							symlevel::save();
							db->del("c_" + name);
							auto del = itSS->first;
							chests.erase(del);
							SELECT_UNPOINT._map.erase(sp);
							return;
						}
						else if (itSS->second.name != offPlayer::getRealName(sp)) {
							wp.sendText(u8"你没有权限解锁此箱子", RAW);
							SELECT_UNPOINT._map.erase(sp);
							return;
						}
					}
				}
			}
		}
	}
	for (auto ita = chests.begin(); ita != chests.end(); ++ita) {
		if (ita->second.uniqid == name) {
			if (ita->second.name != offPlayer::getRealName(sp)) {
				wp.sendText(u8"你没有权限打开此箱子", RAW);
				return;
			}
		}
	}
	return original(a1, a2);
}
THook(void, "?onRemoved@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z",
	ChestBlockActor* a1, BlockSource* a2) {
	auto pos = a1->getPosition();
	auto dimid = SymBlockSource::getDimensionId(a2);
	auto name = LockChest::getPosName(pos.x, pos.y, pos.z, dimid);
	string val;
	if ((db->get(string("c_") + name, val))) {
		for (auto it = chests.begin(); it != chests.end(); ++it) {
			if (it->second.uniqid == name) {
				auto del = it->first;
				auto playername = it->second.name;
				vector<Player*> plist = liteloader::getAllPlayers();
				WPlayer A;
				for (auto p : plist) {
					if (offPlayer::getRealName(p) == playername) {
						A = WPlayer{ *p };
						break;
					}
				}
				if(A){
					symlevel::save();
					db->del("c_" + name);
					chests.erase(del);
					A.sendText(u8"箱子被破坏，已自动解锁!",RAW);
				return original(a1, a2);
				}
				symlevel::save();
				db->del("c_" + name);
				chests.erase(del);
			}
		}
		return original(a1, a2);
	}
	return original(a1, a2);
}

THook(bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* a1,  Actor* a2, BlockPos* pos, ItemStackBase* a4, bool a5) {
	auto sp = MakeSP(a2);
	if (sp) {
		auto wp = WPlayer(*(ServerPlayer*)a2);
		auto blk = SymBlockSource::getBlock(a1, pos);
		auto bl = offBlock::getLegacyBlock(blk);
		auto blname = offBlockLegacy::getFullName(bl);
		if (blname == "minecraft:chest" || blname == "minecraft:trapped_chest" || blname == "minecraft:ender_chest") {
			auto dimid = SymBlockSource::getDimensionId(a1);
			auto name = LockChest::getPosName(pos->x, pos->y, pos->z, dimid);
			string val;
			for (auto it = chests.begin(); it != chests.end(); ++it) {
				if (it->second.uniqid == name) {
					if (it->second.name == offPlayer::getRealName(sp)) {
						return original(a1, a2, pos, a4, a5);
					}
					else if (it->second.name != offPlayer::getRealName(sp)) {
						wp.sendText(u8"你没有权限破坏此箱子", RAW);
						return false;
					}
				}
			}
		}
	}
	return original(a1, a2, pos, a4, a5);
}

THook(bool, "?canDestroy@WitherBoss@@SA_NAEBVBlock@@@Z", Block* sss) {
	auto bl = offBlock::getLegacyBlock(sss);
	auto blname = offBlockLegacy::getFullName(bl);
	if (blname == "minecraft:chest" || blname == "minecraft:trapped_chest") {
		return false;
	}
	return original(sss);
}

THook(bool, "?canDestroyBlock@WitherSkull@@UEBA_NAEBVBlock@@@Z", void* a1, Block* sss) {
	auto bl = offBlock::getLegacyBlock(sss);
	auto blname = offBlockLegacy::getFullName(bl);
	if (blname == "minecraft:chest" || blname == "minecraft:trapped_chest" || blname == "minecraft:ender_chest") {
		return false;
	}
	return original(a1, sss);
}

THook(void, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
	Level* _this, BlockSource* a2, Actor* a3, Vec3* a4, float a5, bool a6, bool a7, float a8, bool a9) {
		auto px = (int)a4->x;
		auto py = (int)a4->y;
		auto pz = (int)a4->z;
		if (px < 0)px = px - 1;
		if (pz < 0)pz = pz - 1;
		for (int i = -5; i < 6; ++i) {
			for (int j = -5; j < 6; ++j) {
				for (int o = -5; o < 6; ++o) {
					auto dimid = SymBlockSource::getDimensionId(a2);
					auto name = LockChest::getPosName(px + i, py + o, pz + j, dimid);
					string val;
					if ((db->get(string("c_") + name, val))) {
						return original(_this, a2, a3, a4, a5, a6, 0, a8, a9);
					}
				}
			}
		}
	return original(_this, a2, a3, a4, a5, a6, a7, a8, a9);
}

THook(void, "?explode@RespawnAnchorBlock@@CAXAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* a1, BlockPos* a2, BlockSource* a3, Level* a4) {
	for (int i = -7; i < 8; ++i) {
		for (int j = -7; j < 8; ++j) {
			for (int o = -7; o < 8; ++o) {
				auto dimid = a1->getDimensionId();
				auto name = LockChest::getPosName(a2->x + i + o, a2->y, a2->z + j, dimid);
				string val;
				if ((db->get(string("c_") + name, val))) {
					return;
				}
			}
		}
		}
	return original(a1, a2, a3, a4);
}


void load() {
	db->iter([](string_view key, string_view val)->bool {
		if (key._Starts_with("c_")) {
			LockChest np;
			RBStream rs(val);
			rs.apply(np);
			auto eid = np.eid;
			chests.emplace(eid, std::move(np));
		}
		return true;
		});
}


void entry() {
	db = MakeKVDB(GetDataPath("LockChest"));
	string val;
	if (db->get("CID", val)) {
		memcpy(&CID, val.data(), 8);
	}
	load();
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("chestlock", "chestlock command", 0);
		//CEnum<HGOP_CMD> _1("hg_setrot", { "cmd" });
		CEnum<COP_LOCK> _2("c_lock", { "lock", "unlock" });
		CEnum<COP_LIST> _3("c_list", { "list" });
		//CEnum<HGOP_REMOVE> _4("hg_remove", { "remove" });
		CmdOverload(chestlock, CCMD_LOCK, "op");
		CmdOverload(chestlock, CCMD_LIST, "list", "name");
		//CmdOverload(chestlock, HGCMD_REMOVE, "remove", "name");
		//CmdOverload(chestlock, HGCMD_CMD, "cmd", "name", "data");
		});
}

