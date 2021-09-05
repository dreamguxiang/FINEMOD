#include"pch.h"
#include <api/scheduler/scheduler.h>

std::unique_ptr<KVDBImpl> db;
typedef unsigned long long eid_t;
static eid_t NPCID = 9223372036854775809ull;
enum NPCOP_LIST :int {
	LIST = 1
};
enum NPCOP_ADD :int {
	ADD = 1
};
enum NPCOP_SETROT :int {
	SETROT = 1
};
enum NPCOP_REMOVE :int {
	REMOVE = 1
};
enum NPCOP_DFLAG :int {
	DFLAG = 1
};
string MINECRAFT_ENTITY_TYPE(string x) {
	if (x.find(':') != x.npos) return x;
	else return "minecraft:" + x;
}
struct NPC {
	string name;
	string nametag;
	string type;
	Vec3 pos;
	Vec3 rot;
	eid_t eid;
	string data;
	int dim;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(name, nametag, type, pos, rot, eid, data);
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
		bs.apply(name, nametag, type, pos, rot, eid, data);
		type = MINECRAFT_ENTITY_TYPE(type);
		string val;
		if (db->get(string("npcdata_") + name, val)) {
			flag = getFLAGS(val);
		}
		else {
			flag = SBIT(14) | SBIT(15);
		}
		if (db->get(string("npcdim_") + name, val)) {
			dim = *(int*)val.data();
		}
		else {
			dim = 0;
		}
	}
	void NetAdd(WBStream& bs)const {
		bs.apply(VarULong(eid), VarULong(eid), MCString(type), pos, Vec3{ 0,0,0 }
			, rot, //rotation
			VarUInt(0), //attr
			VarUInt(3), //metadata :3
			VarUInt(0), VarUInt(7), VarULong(flag), //FLAGS:showtag
			VarUInt(81), VarUInt(0), (char)1, //always show tag
			VarUInt(4), VarUInt(4), MCString(nametag), //nametag
			VarUInt(0) //entity link
		);
		/*
		 stream.putUnsignedVarInt(map.size());
		for (int id : map.keySet()) {
			EntityData d = map.get(id);
			stream.putUnsignedVarInt(id);
			stream.putUnsignedVarInt(d.getType());
			switch (d.getType()) {
			case Entity.DATA_TYPE_BYTE:
					stream.putByte(((ByteEntityData) d).getData().byteValue());
					break;
				case Entity.DATA_TYPE_SHORT:
					stream.putLShort(((ShortEntityData) d).getData());
					break;
				case Entity.DATA_TYPE_INT:
					stream.putVarInt(((IntEntityData) d).getData());

	public static final int DATA_TYPE_BYTE = 0;
	public static final int DATA_TYPE_SHORT = 1;
	public static final int DATA_TYPE_INT = 2;
	public static final int DATA_TYPE_FLOAT = 3;
	public static final int DATA_TYPE_STRING = 4;
	public static final int DATA_TYPE_NBT = 5;
	public static final int DATA_TYPE_POS = 6;
	public static final int DATA_TYPE_LONG = 7;
	public static final int DATA_TYPE_VECTOR3F = 8;

		public static final int DATA_FLAGS = 0;//long
		 public static final int DATA_NAMETAG = 4; //string
		  public static final int DATA_ENTITY_AGE = 24; //short
		  public static final int DATA_ALWAYS_SHOW_NAMETAG = 80; // byte

		   public static final int DATA_FLAG_INLOVE = 7;
		   public static final int DATA_FLAG_POWERED = 9;
	public static final int DATA_FLAG_IGNITED = 10;
		  public static final int DATA_FLAG_ONFIRE = 0;
			  public static final int DATA_FLAG_CAN_SHOW_NAMETAG = 14;
	public static final int DATA_FLAG_ALWAYS_SHOW_NAMETAG = 15;

		*/
	}
	void NetRemove(WBStream& bs) const {
		bs.apply(VarULong(eid));
	}
};
std::unordered_map<eid_t, NPC> npcs;
bool NPCCMD_LIST(CommandOrigin const& ori, CommandOutput& outp, MyEnum<NPCOP_LIST>, optional<string>& name) {
	for (auto& i : npcs) {
		if (name.set) {
			if (i.second.name == name.value()) {
				outp.addMessage("NPC " + i.second.name + " tag " + i.second.nametag + " pos " + i.second.pos.toString() + " dim " + S(i.second.dim));
				return true;
			}
		}
		else {
			outp.addMessage("NPC " + i.second.name + " tag " + i.second.nametag + " pos " + i.second.pos.toString() + " dim " + S(i.second.dim));
		}
	}
	return true;
}
void broadcastPKT(Packet& pk, int dimid) {
	auto usr = liteloader::getAllPlayers();
	for (auto i : usr) {
		if (i->getDimensionId() == dimid) {
			auto spp = (ServerPlayer*)i;
			spp->sendNetworkPacket(pk);
		}
	}
}
void broadcastNPCADD(eid_t nid) {
	WBStream ws;
	npcs[nid].NetAdd(ws);
	MyPkt<0xd> pk{ ws };
	broadcastPKT(pk, npcs[nid].dim);
}
void broadcastNPCREMOVE(eid_t nid) {
	WBStream ws;
	npcs[nid].NetRemove(ws);
	MyPkt<14> pk{ ws };
	broadcastPKT(pk, npcs[nid].dim);
}
void __sendADDNPC(WPlayer wp, NPC const& n) {
	WBStream ws;
	n.NetAdd(ws);
	MyPkt<0xd> pk{ ws };
	wp->sendNetworkPacket(pk);
}
void __sendDELNPC(WPlayer wp, NPC const& n) {
	WBStream ws;
	n.NetRemove(ws);
	MyPkt<14> pk{ ws };
	wp->sendNetworkPacket(pk);
}
void resendAllNPC(WPlayer wp) {
	auto dimid = wp.getDimID();
	for (auto const& [id, n] : npcs) {
		if (n.dim == dimid) {
			__sendADDNPC(wp, n);
		}
		else {
			__sendDELNPC(wp, n);
		}
	}
}
bool NPCCMD_ADD(CommandOrigin const& ori, CommandOutput& outp, MyEnum<NPCOP_ADD>, string& name, string& type, string& tag, string& data) {
	NPC npc;
	npc.name = name;
	npc.nametag = tag;
	npc.type = MINECRAFT_ENTITY_TYPE(type);
	npc.data = data;
	npc.eid = NPCID++;
	npc.pos = ori.getWorldPosition();
	npc.rot = { 0,0,0 };//cant get rot :(
	db->put("NPCID", to_view(NPCID));
	WBStream ws;
	ws.apply(npc);
	db->put("npc_" + name, ws);
	int dimid = MakeWP(ori).val().getDimID();
	db->put("npcdim_" + name, to_view(dimid));
	npc.dim = dimid;
	npcs.emplace(NPCID - 1, std::move(npc));
	broadcastNPCADD(NPCID - 1);
	outp.addMessage("added npc " + name);
	return true;
}
bool NPCCMD_REMOVE(CommandOrigin const& ori, CommandOutput& outp, MyEnum<NPCOP_REMOVE>, string& name) {
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (it->second.name == name) {
			db->del("npc_" + name);
			db->del("npcdata_" + name);
			db->del("npcdim_" + name);
			auto del = it->first;
			broadcastNPCREMOVE(it->first);
			npcs.erase(del);
			outp.addMessage("deleted!");
			return true;
		}
	}
	return false;
}
bool NPCCMD_SETROT(CommandOrigin const& ori, CommandOutput& outp, MyEnum <NPCOP_SETROT>, string& name, float a0, float a1, float a2) {
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (it->second.name == name) {
			NPC& np = it->second;
			np.rot = { a0,a1,a2 };
			WBStream ws;
			ws.apply(np);
			db->put("npc_" + it->second.name, ws);
			broadcastNPCREMOVE(np.eid);
			broadcastNPCADD(np.eid);
			return true;
		}
	}
	return false;
}
bool NPCCMD_DFLAG(CommandOrigin const& ori, CommandOutput& outp, MyEnum <NPCOP_DFLAG>, string& name, string& flag) {
	auto Flag = NPC::getFLAGS(flag);
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (it->second.name == name) {
			NPC& np = it->second;
			np.flag = Flag;
			broadcastNPCREMOVE(np.eid);
			broadcastNPCADD(np.eid);
			db->put("npcdata_" + name, flag);
			outp.addMessage(S(flag));
			return true;
		}
	}
	return false;
}
void npcc() {
	Handler::schedule(RepeatingTask([] {
		vector<Player*> plist = liteloader::getAllPlayers();
		for (auto p : plist) {
			resendAllNPC(WPlayer(*(ServerPlayer*)p));
		}
		}, 10));
}
void load() {
	db->iter([](string_view key, string_view val)->bool {
		if (key._Starts_with("npc_")) {
			NPC np;
			RBStream rs(val);
			rs.apply(np);
			auto eid = np.eid;
			npcs.emplace(eid, std::move(np));
		}
		return true;
		});
}
void entry_npc() {
	string val;
	db = MakeKVDB(GetDataPath("npcutil"));
	if (db->get("NPCID", val)) {
		memcpy(&NPCID, val.data(), 8);
	}
	load();
	npcc();
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("npc", "npc command", 1);
		CEnum<NPCOP_SETROT> _1("npc_setrot", { "setrot" });
		CEnum<NPCOP_ADD> _2("npc_add", {"add"});
		CEnum<NPCOP_LIST> _3("npc_list", { "list" });
		CEnum<NPCOP_REMOVE> _4("npc_remove", { "remove" });
		CEnum< NPCOP_DFLAG> _5("npc_dflag", { "dflag" });
		CmdOverload(npc, NPCCMD_ADD,"add","name","type","nametag","lua_fn");
		CmdOverload(npc, NPCCMD_LIST,"list","name");
		CmdOverload(npc, NPCCMD_REMOVE,"remove","name");
		CmdOverload(npc, NPCCMD_SETROT, "setrot","name","pitch", "from_yaw", "to_yaw");
		CmdOverload(npc, NPCCMD_DFLAG, "dflag","name", "flag");
	});
}
THook(int, "?handle@ItemUseOnActorInventoryTransaction@@UEBA?AW4InventoryTransactionError@@AEAVPlayer@@_N@Z", void* thi, ServerPlayer* sp, bool unk) {
	//  public static final int USE_ITEM_ON_ENTITY_ACTION_INTERACT = 0;
	//static final int USE_ITEM_ON_ENTITY_ACTION_ATTACK = 1;
	auto rtid = dAccess<ActorRuntimeID, 104>(thi);
	auto id = dAccess<int, 112>(thi);
	auto it = npcs.find(rtid);
	if (it != npcs.end()) {
	liteloader::runcmdAs(sp, it->second.data.c_str());
	}
	return original(thi, sp, unk);
}

THook(void, "?respawn@Player@@UEAAXXZ",
	Player* self) {
	original(self);
	resendAllNPC(WPlayer(*self));
}

class ChangeDimensionRequest;
THook(bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z", Level* _this, Player* _this_sp, ChangeDimensionRequest* cdimreq) {
	 resendAllNPC(WPlayer(*(ServerPlayer*)_this_sp));
	return  original(_this, _this_sp, cdimreq);
}