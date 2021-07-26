// Bossbar.cpp : 定义 DLL 的导出函数。
//
#include "pch.h"
/*

extern std::unique_ptr<KVDBImpl> db;
void sendNetworkPacket(Player* pl,void* a0) {
	void (Player:: * rv)(void*) const;
	*((void**)&rv) = dlsym("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z");
	return (pl->*rv)(a0);
}
enum BBOP:int {
	SEND=1,
	CLEAR=2
};
enum BBOP2 :int {
	PIN = 1,
	UNPIN = 2,
	LEN=3
};
string pinmsg;
int barlength;
unsigned long long barID = 9223372036854775808ull;
void sendClearBB(vector<Player*> target) {
	WBStream ws;
	ws.apply(VarULong(barID), VarUInt(2));
	MyPkt<0x4a> pk{ ws };
	for (auto i : target) {
		sendNetworkPacket(i,&pk);
	}
}
void sendBB(vector<Player*> target,string const& str) {
	WBStream ws;
	ws.apply(VarULong(barID), VarULong(barID), MCString("minecraft:pig"));
	for (auto i : target) {
		auto& pos = i->getPos();
		WBStream ws2(ws);
		ws2.apply(Vec3{ pos.x, -10, pos.z });
		MyPkt<0xd> pk{ ws2 };
		sendNetworkPacket(i,&pk);
	}
	ws.data.clear();
	ws.apply(VarULong(barID), VarUInt(0), MCString(str), (float)((float)barlength / 100));
	MyPkt<0x4a> pk{ ws };
	for (auto i : target) {
	    sendNetworkPacket(i,&pk);
	}
}
void broadcastBB(bool show = true) {
	auto uview = liteloader::getAllPlayers();
	sendClearBB(uview);
	if (show) sendBB(uview,pinmsg);
}
bool onCMD2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<BBOP> op, CommandSelector<Player>& scl,optional<string>& msg) {
	auto res=scl.results(ori);
	if (!Command::checkHasTargets(res, outp)) {
		return false;
	}
	bool needsend = op== SEND ? 1:0;
	vector<Player*> target;
	for (auto i : res) {
		target.emplace_back(*(ServerPlayer*)i);
	}
	sendClearBB(target);
	if (needsend) {
		sendBB(target, msg.value());
	}
	return true;
}
bool onCMD(CommandOrigin const& ori, CommandOutput& outp, MyEnum<BBOP2> op,optional<string>& msg) {
	switch (op)
	{
	case PIN:
		db->put("BBpinmsg", msg.value());
		pinmsg = msg.value();
		broadcastBB();
		break;
	case UNPIN:
		db->del("BBpinmsg");
		broadcastBB(false);
		pinmsg = "";
		break;
	case LEN:
		int len = std::atoi(msg.value().c_str());
		db->put("BBpinlen", to_view(len));
		barlength = len;
		broadcastBB();
	}
	return true;
}

void entry_bossbar() {
	db =MakeKVDB(GetDataPath("npcutil"));
	db->get("BBpinmsg", pinmsg);
	string val;
	if (db->get("BBpinlen", val)) {
		memcpy(&barlength, val.data(), 4);
	}
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		CEnum<BBOP2> _1("bbop2", { "pin", "unpin","length" });
		CEnum<BBOP> _2("bbop", { "send","clear" });
		MakeCommand("bb", "bossbar command", 1);
		CmdOverload(bb, onCMD, "operation","msg_or_length");
		CmdOverload(bb, onCMD2, "operation", "player","msg");
	});
	Event::addEventListener([](JoinEV ev) {
		WPlayer wp = WPlayer(*(ServerPlayer*)ev.Player);
		if (pinmsg.length()) {
			sendBB({ wp },pinmsg);
		}
	});
	Event::addEventListener([](ChangeDimEV ev) {
		WPlayer wp = WPlayer(*(ServerPlayer*)ev.Player);
		if (pinmsg.length()) {
			sendBB({ wp }, pinmsg);
		}
		});
}
*/