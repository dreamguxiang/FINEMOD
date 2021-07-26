#include "pch.h"
#include<api\regCommandHelper.h>
#include <api/myPacket.h>
typedef unsigned long long eid_t;
static eid_t HGID = 9223372036854775809ull;
constexpr unsigned int H(string_view s)
{
	auto str = s.data();
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) + (*str++); /* times 33 */
	}
	hash &= ~(1 << 31); /* strip the highest bit */
	return hash;
}


string& replace_all_distinct(string& str, const   string& old_value, const   string& new_value)
{
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length()) {
		if ((pos = str.find(old_value, pos)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}
string& replace_all(string& str, const   string& old_value, const   string& new_value)
{
	while (true) {
		string::size_type   pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}
string MINECRAFT_ENTITY_TYPE(string x) {
	if (x.find(':') != x.npos) return x;
	else return "minecraft:" + x;
}
std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	return timestamp;
}
struct NPC {
	string nametag;
	Vec3 pos;
	Vec3 rot;
	eid_t eid;
	float size;
	int dim;
	time_t time;
	string plname;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(nametag, pos, rot, eid,size,time,plname);
	}
	void NetAdd(WBStream& bs)const {
		bs.apply(VarULong(eid), VarULong(eid), MCString("minecraft:pig"), pos, Vec3{ 0,0,0 }
			, rot, //rotation
			VarUInt(0), //attr
			VarUInt(4), //metadata :4
			VarUInt(0), VarUInt(7), VarULong(flag), //FLAGS:showtag
			VarUInt(81), VarUInt(0), (char)1, //always show tag
			VarUInt(4), VarUInt(4), MCString(nametag), //nametag
			VarUInt(0x26), VarUInt(3), size, // 
			VarUInt(0) //entity link
		);
	}
	void NetRemove(WBStream& bs) const {
		bs.apply(VarULong(eid));
	}
};

std::unordered_map<string, NPC> npcs;
void sendADDHG(NPC const& n) {
	WBStream ws;
	n.NetAdd(ws);
	MyPkt<0xd> pk{ ws };
	std::vector<Player*> plist = liteloader::getAllPlayers();
	for (auto p : plist) {
		WPlayer(*(ServerPlayer*)p)->sendNetworkPacket(pk);
	}
}

void sendDELHG( NPC const& n) {
	WBStream ws;
	n.NetRemove(ws);
	MyPkt<14> pk{ ws };
	std::vector<Player*> plist = liteloader::getAllPlayers();
	for (auto p : plist) {
		WPlayer(*(ServerPlayer*)p)->sendNetworkPacket(pk);
	}
}
#include<mc/OffsetHelper.h>
#include <regex>
void removenpc(string name) {
	auto iter = npcs.find(name);
	if (iter != npcs.end())
		iter = npcs.erase(iter);
}

int getTrueLength(string str) {
	int valueLength = 0;
	for (int i = 0; i < str.length(); i++) {
		string temp = str.substr(i, i + 1);
		std::regex express("[\u4e00-\u9fa5]");
		std::match_results<std::string::iterator> results1;
		if (std::regex_match(str.begin(), str.end(), results1, express)) {
			valueLength += 2;
		}
		else {
			valueLength += 1;
		}
	}
	return valueLength;
}

int getEnglishLength(string str) {
	int valueLength = 0;
	for (int i = 0; i < str.length(); i++) {
		string temp = str.substr(i, i + 1);
		std::regex express("[\u4e00-\u9fa5]");
		std::match_results<std::string::iterator> results1;
		if (!std::regex_match(str.begin(), str.end(), results1, express)) {
			valueLength++;
		}
	}
	return valueLength;
}

string chatMessage(string message) {
	string chatMessage = u8"";
	int length = (message.length() % 12) > 0 ? message.length() / 12 + 1 : message.length() / 12;
	string line = u8"¡ìl¨q©¤";
	for (int i = 0; i < 8; i++) {
		line.append(u8"©¤©¤");
	}
	int l = getTrueLength(line) - 3;
	line.append(u8"¨r");
	string lineEnd = "";
	string builder = "";
	for (int i = 0; i < length; i++) {
		string split;
		if (message.length() < (i * 12 + 12)) {
			split = message.substr(i * 12);
		}
		else {
			split = message.substr(i * 12, i * 12 + 12);
		}
		int addK = (getTrueLength(split) - getEnglishLength(split)) / 2;
		int adds = getEnglishLength(split) + (addK * 2);
		std::regex express("[\u4e00-\u9fa5]");
		std::match_results<std::string::iterator> results1;
		if (!std::regex_match(split.substr(0, 1).begin(), split.substr(0, 1).end(), results1, express)) {
			builder.append(u8"¡ìr©¦¡ìr ¡ìe");
		}
		else {
			builder.append(u8"¡ìr©¦¡ìr   ¡ìe");
		}
		builder.append(split);
		for (int c = 0; c < l - adds + 2; c++) {
			std::match_results<std::string::iterator> results2;
			if (!std::regex_match(split.substr(split.length()).begin(), split.substr(split.length()).end(), results2, express) || addK > getEnglishLength(split)) {
				builder.append("");
			}
			else {
				builder.append("");
			}
		}
		builder.append(u8"¡ìr|").append("\n");
	}
	lineEnd.append(u8"¨t");
	lineEnd.append(u8"¡ìr¡ìl");
	for (int a = 0; a < 8; a++) {
		lineEnd.append(u8"©¤©¤");
	}
	lineEnd.append(u8"¨s");
	string k = " ";
	for (int a = 0; a < 19; a++) {
		k.append(" ");
	}
	chatMessage.append(line).append("\n").append(builder).append(lineEnd).append("\n");
	return chatMessage;
}

THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVTextPacket@@@Z",
	void* self, NetworkIdentifier* id, void* text) {
	auto pl = SymCall("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
		Player*, void*, void*, char)(self, id, *(char*)((uintptr_t)text + 16));
	auto msg = std::string(*(std::string*)((uintptr_t)text + 88));
	WPlayer wp = WPlayer{ *pl };
	if (npcs.find(offPlayer::getRealName(pl)) != npcs.end()) {
		sendDELHG(npcs[offPlayer::getRealName(pl)]);
		removenpc(offPlayer::getRealName(wp));
	}
	NPC npc;
	npc.nametag = chatMessage(msg);
	npc.eid = HGID++;
	npc.size = (float)0.0;
	npc.pos = pl->getPos().add(0,0.5,0);
	npc.time = getTimeStamp();
	npcs.emplace(offPlayer::getRealName(pl), std::move(npc));
	sendADDHG( npc);
	return original(self, id, text);
}

THook(void, "?move@Player@@UEAAXAEBVVec3@@@Z", Player* pl, Vec3* a2) {
	if (npcs.find(offPlayer::getRealName(pl)) != npcs.end()) {
		WPlayer wp = WPlayer{ *pl };
		auto plnames = offPlayer::getRealName(pl);
		if (getTimeStamp() - npcs[plnames].time <= 5000) {
			sendDELHG( npcs[plnames]);
			npcs[plnames].pos = pl->getPos().add(0, 0.5, 0);
			sendADDHG( npcs[plnames]);
		}
		else {
			sendDELHG( npcs[plnames]);
			removenpc(offPlayer::getRealName(wp));
		}
	}
	return original(pl, a2);
}
