#include"pch.h"
#include"api.h"
#include "llmoney.h"
using namespace std;
std::unique_ptr<KVDBImpl> db;
string rx;
string rxs;
string signinlxqdlists;
string signinnumlists;
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
static string api[] = {
	"%time%",
	"%llmoney%",
	"%playername%" ,
	"%serverversion%",
	"%serverprotocolversion%",
	"%playerpermlevel%",
	"%diskdrivetype%",
	"%cputype%",
	"&n",
	"%ostype%",
	"%moneylist%",
	"%killmoblist%",
	"%signinlxqdlist%",
	"%signinnumlist%"
};
enum HGOP_LIST :int {
	LIST = 1
};

enum HGOP_ADD :int {
	ADD = 1
};

enum HGOP_CMD :int {
	CMD = 1
};

enum HGOP_REMOVE :int {
	REMOVE = 1
};

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

struct NPC {
	string name;
	string nametag;
	string type;
	Vec3 pos;
	Vec3 rot;
	eid_t eid;
	string data;
	float size;
	int dim;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(name, nametag, type, pos, rot, eid, data,size);
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
		bs.apply(name, nametag, type, pos, rot, eid, data,size);
		type = MINECRAFT_ENTITY_TYPE(type);
		string val;
		if (db->get(string("hgdata_") + name, val)) {
			flag = getFLAGS(val);
		}
		else {
			flag = SBIT(14) | SBIT(15);
		}
		if (db->get(string("hgdim_") + name, val)) {
			dim = *(int*)val.data();
		}
		else {
			dim = 0;
		}
	}
	void NetAdd(WBStream& bs,WPlayer& wp)const {
		auto namee = nametag;
		auto sp = MakeSP(wp);
		string val;
		for (auto& m : api) {
			switch (H(m)){
				case H("%time%"):
					replace_all_distinct(namee, m, gettime());
					break;
				case H("%llmoney%"):
					replace_all_distinct(namee, m, getmoney(wp));
					break;
				case H("%playername%"):
					replace_all_distinct(namee, m, offPlayer::getRealName(wp));
					break;
				case H("%serverversion%"):
					replace_all_distinct(namee, m, getversion());
					break;
				case H("%serverprotocolversion%"):
					replace_all_distinct(namee, m, getProtocolVersion());
					break;
				case H("%playerpermlevel%"):
					replace_all_distinct(namee, m, getplayerpermlevel(wp));
					break;
				case H("%diskdrivetype%"):
					replace_all_distinct(namee, m, diskdrivename);
					break;
				case H("%cputype%"):
					replace_all_distinct(namee, m, cpuname);
					break;
				case H("%ostype%"):
					replace_all_distinct(namee, m, osname);
					break;
				case H("%moneylist%"):
					replace_all_distinct(namee, m, rx);
					break;
				case H("%killmoblist%"):
					replace_all_distinct(namee, m, rxs);
					break;
				case H("%signinnumlist%"):
					replace_all_distinct(namee, m, signinnumlists);
					break;
				case H("%signinlxqdlist%"):
					replace_all_distinct(namee, m, signinlxqdlists);
					break;
				case H("&n"):
					replace_all_distinct(namee, m, "\n");
					break;
				default:
					return;
				}
		}
		bs.apply(VarULong(eid), VarULong(eid), MCString("minecraft:pig"), pos, Vec3{ 0,0,0 }
			, rot, //rotation
			VarUInt(0), //attr
			VarUInt(4), //metadata :4
			VarUInt(0), VarUInt(7), VarULong(flag), //FLAGS:showtag
			VarUInt(81), VarUInt(0), (char)1, //always show tag
			VarUInt(4), VarUInt(4), MCString(namee), //nametag
			VarUInt(0x26), VarUInt(3), size, // 
			VarUInt(0) //entity link
		);
	}
	void NetRemove(WBStream& bs) const {
		bs.apply(VarULong(eid));
	}
};

std::unordered_map<eid_t, NPC> npcs;
bool HGCMD_LIST(CommandOrigin const& ori, CommandOutput& outp, MyEnum<HGOP_LIST>, optional<string>& name) {
	for (auto& i : npcs) {
		if (name.set) {
			if (i.second.name == name.value()) {
				outp.addMessage("NAMEID:" + i.second.name + " Tag: " + i.second.nametag + u8"§r Pos: " + i.second.pos.toString() + " Dim: " + S(i.second.dim));
				return true;
			}
		}
		else {
			if (strstr(i.second.name.c_str(), "_cmd") == NULL) {
				outp.addMessage("NAMEID: " + i.second.name + " Tag: " + i.second.nametag + u8" §rPos: " + i.second.pos.toString() + " Dim: " + S(i.second.dim));
			}
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

void broadcastHGADD(eid_t nid,WPlayer& wp) {
	WBStream ws;
	npcs[nid].NetAdd(ws,wp);
	MyPkt<0xd> pk{ ws };
	broadcastPKT(pk, npcs[nid].dim);
}

void broadcastHGREMOVE(eid_t nid) {
	WBStream ws;
	npcs[nid].NetRemove(ws);
	MyPkt<14> pk{ ws };
	broadcastPKT(pk, npcs[nid].dim);
}

void __sendADDHG(WPlayer wp, NPC const& n) {
	WBStream ws;
	n.NetAdd(ws,wp);
	MyPkt<0xd> pk{ ws };
	wp->sendNetworkPacket(pk);
}

void __sendDELHG(WPlayer wp, NPC const& n) {
	WBStream ws;
	n.NetRemove(ws);
	MyPkt<14> pk{ ws };
	wp->sendNetworkPacket(pk);
}

void resendAllHG(WPlayer wp) {
	auto dimid = wp.getDimID();
	for (auto& [id, n] : npcs) {
		if (n.dim == dimid) {
			__sendADDHG(wp, n);
		}
		else {
			__sendDELHG(wp, n);
		}
	}
}


bool HGCMD_ADD(CommandOrigin const& ori, CommandOutput& outp, MyEnum<HGOP_ADD>, string& name, string& tag, optional<float>& px, optional<float>& py, optional<float>& pz) {
	auto wp = MakeWP(ori).val();
	NPC npc;
	for (auto& m : api) {
		auto idx = tag.find(m);
		if (idx != string::npos) {
			db->put("hgdt_" + name, "dt");
		}
	}
	npc.nametag = tag;
	npc.name = name;
	npc.eid = HGID++;
	npc.size = (float)0.0;
	if (px.set && pz.set && py.set) {
		Vec3 poss = { px.val(),py.val() ,pz.val() };
		npc.pos = poss;
	}
	else if(px.set == false && py.set == false && pz.set == false) {
		npc.pos = ori.getWorldPosition();
	}
	else if (px.set == true && py.set == false || pz.set == false) {
		outp.addMessage(u8"§l§6[§eHologramGlows§6]§r pos error");
		return false;
	}
	else if (px.set == true && py.set == true && pz.set == false) {
		outp.addMessage(u8"§l§6[§eHologramGlows§6]§r pos error");
		return false;
	}
	npc.rot = { 0,0,0 };//cant get rot :(
	db->put("HGID", to_view(HGID));
	WBStream ws;
	ws.apply(npc);
	db->put("hg_" + name, ws);
	int dimid = MakeWP(ori).val().getDimID();
	db->put("hgdim_" + name, to_view(dimid));
	npc.dim = dimid;
	npcs.emplace(HGID - 1, std::move(npc));
	broadcastHGADD(HGID - 1,wp);
	outp.addMessage(u8"§l§6[§eHologramGlows§6]§r added success\nNAMEID: "+ name +" \nX:" + std::to_string(npc.pos.x) + " \nY:" + std::to_string(npc.pos.y)+" \nZ:"+std::to_string(npc.pos.z));
	return true;
}

bool HGCMD_REMOVE(CommandOrigin const& ori, CommandOutput& outp, MyEnum<HGOP_REMOVE>, string& name) {
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (it->second.name == name) {
			for (auto its = npcs.begin(); its != npcs.end(); ++its) {
				string val;
				if ((db->get(string("hgcmd_") + name, val))) {
					string kll = val.data();
					if (kll == its->second.name) {
						db->del("hg_" + its->second.name);
						db->del("hgdata_" + its->second.name);
						db->del("hgdim_" + its->second.name);
						db->del("hgcmd_" + its->second.name);
						db->del("hgdt_" + name);
						auto dels = its->first;
						broadcastHGREMOVE(its->first);
						npcs.erase(dels);
						break;
					}
				}
			}
			db->del("hg_" + name);
			db->del("hgdata_" + name);
			db->del("hgdim_" + name);
			db->del("hgcmd_" + name);
			db->del("hgdt_" + name);
			auto del = it->first;
			broadcastHGREMOVE(it->first);
			npcs.erase(del);
			outp.addMessage(u8"§l§6[§eHologramGlows§6]§r deleted "+ name);
			return true;
		}
	}
	return false;
}

bool HGCMD_CMD(CommandOrigin const& ori, CommandOutput& outp, MyEnum <HGOP_CMD>, string& name, string& data) {
	auto wp = MakeWP(ori).val();
	string val;
	if ((db->get(string("hgcmd_") + name, val))) {
		outp.addMessage(u8"§l§6[§eHologramGlows§6]§r You've already added cmd");
		return true;
	}
	for (auto it = npcs.begin(); it != npcs.end(); ++it) {
		if (it->second.name == name) {
			auto ppp = it->second.pos;
			auto bx = ppp.x;
			auto by = ppp.y;
			auto bz = ppp.z;
			auto ppy = by + (float)0.285;
			string names = "_cmd";
			string namess = name;
			namess += names;
			NPC npc;
			npc.name = namess;
			npc.data = data;
			npc.nametag = "";
			npc.eid = HGID++;
			npc.size = (float)0.3;
			Vec3 poss = { bx, ppy ,bz };
			npc.pos = poss;
			npc.rot = { 0,0,0 };
			npc.flag = SBIT(6);
			db->put("HGID", to_view(HGID));
			WBStream ws;
			ws.apply(npc);
			db->put("hg_" + namess, ws);
			int dimid = MakeWP(ori).val().getDimID();
			db->put("hgdim_" + namess, to_view(dimid));
			npc.dim = dimid;
			npcs.emplace(HGID - 1, std::move(npc));
			broadcastHGADD(HGID - 1,wp);
			db->put("hgcmd_" + name, namess);
			outp.addMessage(u8"§l§6[§eHologramGlows§6]§r set success \nNAMEID:" + name+"\nCMD:"+ data);
			return true;
		}
	}
}

void npcc() {
	Handler::schedule(RepeatingTask([] {
		vector<Player*> plist = liteloader::getAllPlayers();
		for (auto p : plist) {
			resendAllHG(WPlayer(*(ServerPlayer*)p));
		}
		}, 2));
}
void npccs() {
	Handler::schedule(RepeatingTask([] {
		auto all = Money::getAllMoney();
		int num = 1;
		int j = 10;
		rx = "";
		string tx;
		if (all.size() < 10) {
			j = all.size();
		}
		for (auto i = 0; i < j; ++i) {
			auto name = all[i].first;
			auto money = all[i].second;
			switch (num) {
			case 1:
				tx = u8"§l§eFirst   ";
				break;
			case 2:
				tx = u8"§l§6Second  ";
				break;
			case 3:
				tx = u8"§l§bThird   ";
				break;
			case 4:
				tx = u8"§l§aFourth  ";
				break;
			case 5:
				tx = u8"§l§1Fifth   ";
				break;
			case 6:
				tx = u8"§l§9Sixth   ";
				break;
			case 7:
				tx = u8"§l§4Seventh ";
				break;
			case 8:
				tx = u8"§l§3Eighth  ";
				break;
			case 9:
				tx = u8"§l§cNinth   ";
				break;
			case 10:
				tx = u8"§l§cTenth   ";
				break;
			}
			rx += tx + name + "  " + to_string(money) + "\n";
			num++;
		}
		}, 120));
}

#include <llhunter.h>

bool GreaterSort(killmoblist const&  a, killmoblist const&  b) {
	return (a.second > b.second);
}

void killlist() {
	auto all = Hunter::getkillmoblist();
	std::sort(all.begin(), all.end(), GreaterSort);
	int num = 1;
	int j = 10;
	rxs = "";
	string tx;
	if (all.size() < 10) {
		j = all.size();
	}
	for (auto i = 0; i < j; ++i) {
		auto name = all[i].first;
		auto money = all[i].second;
		switch (num) {
		case 1:
			tx = u8"§l§eFirst   ";
			break;
		case 2:
			tx = u8"§l§6Second  ";
			break;
		case 3:
			tx = u8"§l§bThird   ";
			break;
		case 4:
			tx = u8"§l§aFourth  ";
			break;
		case 5:
			tx = u8"§l§1Fifth   ";
			break;
		case 6:
			tx = u8"§l§9Sixth   ";
			break;
		case 7:
			tx = u8"§l§4Seventh ";
			break;
		case 8:
			tx = u8"§l§3Eighth  ";
			break;
		case 9:
			tx = u8"§l§cNinth   ";
			break;
		case 10:
			tx = u8"§l§cTenth   ";
			break;
		}
		rxs += tx + name + "  " + to_string(money) + "\n";
		num++;
	}
}

#include <FineSignIn.h>
void ssigninnumlist() {
	auto all = finesignin::getsigninnumlist();
	std::sort(all.begin(), all.end(), GreaterSort);
	int num = 1;
	int j = 10;
	signinnumlists = "";
	string tx;
	if (all.size() < 10) {
		j = all.size();
	}
	for (auto i = 0; i < j; ++i) {
		auto name = all[i].first;
		auto money = all[i].second;
		switch (num) {
		case 1:
			tx = u8"§l§eFirst   ";
			break;
		case 2:
			tx = u8"§l§6Second  ";
			break;
		case 3:
			tx = u8"§l§bThird   ";
			break;
		case 4:
			tx = u8"§l§aFourth  ";
			break;
		case 5:
			tx = u8"§l§1Fifth   ";
			break;
		case 6:
			tx = u8"§l§9Sixth   ";
			break;
		case 7:
			tx = u8"§l§4Seventh ";
			break;
		case 8:
			tx = u8"§l§3Eighth  ";
			break;
		case 9:
			tx = u8"§l§cNinth   ";
			break;
		case 10:
			tx = u8"§l§cTenth   ";
			break;
		}
		signinnumlists += tx + name + "  " + to_string(money) + "\n";
		num++;
	}
}

void ssigninlxqdlist() {
	auto all = finesignin::getsigninlxqdlist();
	std::sort(all.begin(), all.end(), GreaterSort);
	int num = 1;
	int j = 10;
	signinlxqdlists = "";
	string tx;
	if (all.size() < 10) {
		j = all.size();
	}
	for (auto i = 0; i < j; ++i) {
		auto name = all[i].first;
		auto money = all[i].second;
		switch (num) {
		case 1:
			tx = u8"§l§eFirst   ";
			break;
		case 2:
			tx = u8"§l§6Second  ";
			break;
		case 3:
			tx = u8"§l§bThird   ";
			break;
		case 4:
			tx = u8"§l§aFourth  ";
			break;
		case 5:
			tx = u8"§l§1Fifth   ";
			break;
		case 6:
			tx = u8"§l§9Sixth   ";
			break;
		case 7:
			tx = u8"§l§4Seventh ";
			break;
		case 8:
			tx = u8"§l§3Eighth  ";
			break;
		case 9:
			tx = u8"§l§cNinth   ";
			break;
		case 10:
			tx = u8"§l§cTenth   ";
			break;
		}
		signinlxqdlists += tx + name + "  " + to_string(money) + "\n";
		num++;
	}
}

THook2("玩家保存", void, "?saveLevelData@Level@@UEAAXXZ", Level* a1) {
	killlist();
	ssigninnumlist();
	ssigninlxqdlist();
	return original(a1);
}
void load() {
	db->iter([](string_view key, string_view val)->bool {
		if (key._Starts_with("hg_")) {
			NPC np;
			RBStream rs(val);
			rs.apply(np);
			auto eid = np.eid;
			npcs.emplace(eid, std::move(np));
		}
		return true;
		});
}


const string version = "210610";
int updateCheck();
#include "api.h"

void entry_npc() {
	std::cout << " _    _       _                                  _____ _                   " << endl
		<< "| |  | |     | |                                / ____| |                  " << endl
		<< "| |__| | ___ | | ___   __ _ _ __ __ _ _ __ ___ | |  __| | _____      _____" << endl
		<< "|  __  |/ _ \\| |/ _ \\ / _` | '__/ _` | '_ ` _ \\| | |_ | |/ _ \\ \\ /\\ / / __|" << endl
		<< "| |  | | (_) | | (_) | (_| | | | (_| | | | | | | |__| | | (_) \\ V  V /\\__ \\" << endl
		<< "|_|  |_|\\___/|_|\\___/ \\__, |_|  \\__,_|_| |_| |_|\\_____|_|\\___/ \\_/\\_/ |___/ " << endl
		<< "                       __/ |                                               " << endl
		<< "                      |___/                                                 " << endl
		<< "[HologramGlows] HologramGlows "<< version <<" Loaded,By DreamGuXiang\n"
		<< "[HologramGlows] Target: 1.17.0.03  Build Date [" << __TIMESTAMP__  <<u8"] @LiteLoader" << endl;

	string val;
	db = MakeKVDB(GetDataPath("HologramGlows"));
	if (db->get("HGID", val)) {
		memcpy(&HGID, val.data(), 8);
	}
	load();
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("hg", "HologramGlows command", 1);
		CEnum<HGOP_CMD> _1("hg_setrot", { "cmd" });
		CEnum<HGOP_ADD> _2("hg_add", { "add" });
		CEnum<HGOP_LIST> _3("hg_list", { "list" });
		CEnum<HGOP_REMOVE> _4("hg_remove", { "remove" });
		CmdOverload(hg, HGCMD_ADD, "add", "id", "tag","x","y","z");
		CmdOverload(hg, HGCMD_LIST, "list", "name");
		CmdOverload(hg, HGCMD_REMOVE, "remove", "name");
		CmdOverload(hg, HGCMD_CMD, "cmd", "name", "data");
		});
	Event::addEventListener([](ServerStartedEV) {
		updateCheck();
		npccs();
		killlist();
		ssigninnumlist();
		ssigninlxqdlist();
		npcc();
		});
}

THook(int, "?handle@ItemUseOnActorInventoryTransaction@@UEBA?AW4InventoryTransactionError@@AEAVPlayer@@_N@Z", void* thi, ServerPlayer* sp, bool unk) {
	auto rtid = dAccess<ActorRuntimeID, 104>(thi);
	auto id = dAccess<int, 112>(thi);
	if (!id) return original(thi, sp, unk);;
	auto it = npcs.find(rtid);
	if (it != npcs.end()) {
		auto idx = it->second.name.find("_cmd");
		if (idx != string::npos) {
			liteloader::runcmdAs(sp, it->second.data.c_str());
		}
	}
	return original(thi, sp, unk);
}

THook(void, "?respawn@Player@@UEAAXXZ",
	Player* self) {
	original(self);
	resendAllHG(WPlayer(*self));
}

class ChangeDimensionRequest;
THook(bool, "?_playerChangeDimension@Level@@AEAA_NPEAVPlayer@@AEAVChangeDimensionRequest@@@Z", Level* _this, Player* _this_sp, ChangeDimensionRequest* cdimreq) {
	resendAllHG(WPlayer(*(ServerPlayer*)_this_sp));
	return  original(_this, _this_sp, cdimreq);
}
