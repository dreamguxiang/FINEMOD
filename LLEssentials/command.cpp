#include "pch.h"
#include<api\regCommandHelper.h>
#include<api/basicEvent.h>
#include "commad.h"
#include <stddef.h>
#include <iostream>
#include <string>
#include "BDS.h"
#include "Helper.h"
#include <string_view>
#include "tickdo.h"
#include "tpa.h"
#include <api\scheduler\scheduler.h>
#include <cstdarg>
#include <mutex>

using namespace std;
unique_ptr<KVDBImpl> dbcname;
playerMap<string> ORIG_NAME;
unordered_map<string, string> CNAME;
string  ss = "";
static LangPack LangP("plugins/LLEssentials/langpack/zh_cn.json");
std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}
void loadCNAME() {
	dbcname = MakeKVDB(GetDataPath("LLhelper"), false);
	dbcname->iter([](string_view k, string_view v) {
		if (!k._Starts_with("b_"))
			CNAME.emplace(k, v);
		return true;
		});
}
bool onRunAS(CommandOrigin const& ori, CommandOutput& outp, CommandSelector<Player>& p, CommandMessage& cm) {
	auto res = p.results(ori);
	if (!Command::checkHasTargets(res, outp)) return false;
	string cmd = cm.get(ori);
	for (auto i : res) {
		WPlayer wp{ *i };
		wp.runcmd(cmd);
	}
	return true;
}
bool oncmd_gmode(CommandOrigin const& ori, CommandOutput& outp, CommandSelector<Player>& s, int mode) {
	auto res = s.results(ori);
	if (!Command::checkHasTargets(res, outp)) return false;
	for (auto i : res) {
			setPlayerGameType(i, mode);
	}
	return true;
}

enum class BANOP :int {
	ban = 1,
	unban = 2,
	banip = 3,
	deviceid = 4
};
enum class BANOP_LIST :int {
	list = 1
};

void LOWERSTRING(string& S) {
	for (auto& i : S) {
		i = tolower(i);
	}
}
void addBanEntry(string const& entry, time_t timediff) {
	time_t next = timediff == 0 ? 0 : (time(0) + timediff);
	dbcname->put("b_" + entry, to_view(next));
}
optional<time_t> getBanEntry(string const& entry) {
	string val;
	if (dbcname->get("b_" + entry, val)) return { *(time_t*)val.data() };
	return {};
}
void removeBanEntry(string const& entry) {
	dbcname->del("b_" + entry);
}

bool onCMD_BanList(CommandOrigin const& ori, CommandOutput& outp, MyEnum<BANOP_LIST>) {
	dbcname->iter([&](string_view key, string_view val)->bool {
		if (key._Starts_with("b_")) {
			string banned{ key.substr(2) };
			if (banned.find('.') != banned.npos) {
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r Ban IP List \n" + banned + " " + std::to_string(*(time_t*)val.data()));
			}
			else {
				xuid_t xid;
				std::stringstream ss{ banned };
				ss >> xid;
				auto strname = XIDREG::id2str(xid);
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r Ban Player List\n"+ banned + " (" + (strname.set ? strname.val() : "") + ") " + std::to_string(*(time_t*)val.data()));
			}
		}
		return true;
		});
	return true;
}
bool onCMD_Ban(CommandOrigin const& ori, CommandOutput& outp, MyEnum<BANOP> op, string& entry, optional<int>& time) {
	LOWERSTRING(entry);
	switch (op.val)
	{
	case BANOP::banip: {
		addBanEntry(entry, time.set ? time.val() : 0);
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r ban IP:" + QUOTE(entry) + " success");
		return true;
	}
	case BANOP::deviceid: {
		addBanEntry(entry, time.set ? time.val() : 0);
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r ban deviceid:" + QUOTE(entry) + " success");
		return true;
	}
	case BANOP::ban: {
		addBanEntry(S(XIDREG::str2id(entry).val()), time.set ? time.val() : 0);
		vector<Player*> plist = liteloader::getAllPlayers();
		Player* A = nullptr;
		for (auto p : plist) {
			auto nm = offPlayer::getRealName(p);
			LOWERSTRING(nm);
			if (nm == entry) {
				A = p;
				break;
			}
		}
		if (A) {
			forceKick(A, "banned");
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 封禁 " + QUOTE(entry) + " 成功");
			return true;
		}
		else {
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 封禁 " + QUOTE(entry) + " 成功");
		}
		return true;
	}
				   break;
	case BANOP::unban: {
		if (getBanEntry(entry).set) {
			removeBanEntry(entry);
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 解除封禁 " + QUOTE(entry) + " 成功");
			return true;
		}
		else {
			auto XID = XIDREG::str2id(entry);
			if (!XID.set) {
				outp.error(u8"§l§6[§eMCBE §bFINE§6]§r 玩家没有被封禁");
				return false;
			}
			else {
				removeBanEntry(S(XID.val()));
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 解除封禁 " + QUOTE(entry) + " 成功");
				return true;
			}
		}
	}
					 break;
	default:
		break;
	}
	return false;
}

bool onCMD_skick(CommandOrigin const& ori, CommandOutput& outp, string& target, string& msg) {
	string playername = target;
	vector<Player*> plist = liteloader::getAllPlayers();
	Player* A = nullptr;
	for (auto p : plist) {
		if (offPlayer::getRealName(p) == playername) {
			A = p;
			break;
		}
	}
	if(A){
		forceKick(A,msg);
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 踢出成功");
		return true;
	}
	else
	{
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("ban.notplayer"));
	}
}

bool oncmd_kickall(CommandOrigin const& ori, CommandOutput& outp) {
	vector<Player*> plist = liteloader::getAllPlayers();
	for (auto p : plist) {
			forceKick(p, u8"服务器正在重启ing\n服务器正在重启ing\n服务器正在重启ing\n服务器正在重启ing\n服务器正在重启ing\n服务器正在重启ing");
		}
	std::thread t([] {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		liteloader::runcmdEx("stop");
		});
	t.detach();
	return true;
}

bool oncmd_vanish(CommandOrigin const& ori, CommandOutput& outp) {
	auto wp = MakeWP(ori).val();
	auto sp = MakeSP(ori);
	VarULong ul(ZigZag(wp->getUniqueID().id));
	WBStream ws;
	ws.apply(ul);
	MyPkt<14> pk{ ws };
	vector<Player*> plist = liteloader::getAllPlayers();
	for (auto p : plist) {
		if (p != sp) {
			auto spp = (ServerPlayer*)p;
			spp->sendNetworkPacket(pk);
		}
	}
	outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("vanish.success"));
	return true;
}

enum class CNAMEOP :int {
	set = 1,
	remove = 2
};


bool onCMD_CNAME(CommandOrigin const& ori, CommandOutput& p, MyEnum<CNAMEOP> op, string& src, optional<string>& name) {
		string playername = src;
		vector<Player*> plist = liteloader::getAllPlayers();

		Player* A = nullptr;
		for (auto p : plist) {
			if (offPlayer::getRealName(p) == playername) {
				A = p;
				break;
			}
		}
		if (A) {
			if (op == CNAMEOP::set) {
				auto& str = name.val();
				for (int i = 0; i < str.size(); ++i) {
					if (str[i] == '^') str[i] = '\n';
				}
				CNAME[src] = str;
				dbcname->put(src, str);
				optional<WPlayer> aa = WPlayer(*(ServerPlayer*)A);
				if (!aa.set) {
					p.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("cname.set.notplayer"));
					return false;
				}
				aa.val()->setName(str);
				ORIG_NAME[aa.val().v] = offPlayer::getRealName(aa.val());
				p.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("cname.set.success"));
			}
			else {
				CNAME.erase(src);
				dbcname->del(src);
				optional<WPlayer> aa = WPlayer(*(ServerPlayer*)A);
				if (!aa.set) {
					p.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("cname.del.notplayer"));
					return false;
				}
				aa.val()->setName(src);
				ORIG_NAME._map.erase(aa.val().v);
				p.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("cname.del.success"));
			}
			return true;
		}
}


bool onCMD_Trans(CommandOrigin const& ori, CommandOutput& outp, CommandSelector<Player>& p, string& host, optional<int> port) {
	int P = port.set ? port.val() : 19132;
	auto res = p.results(ori);
	if (!Command::checkHasTargets(res, outp)) return false;
	WBStream ws;
	ws.apply(MCString(host), (unsigned short)P);
	MyPkt<0x55, false> trpk(ws);
	for (auto i : res) {
		((ServerPlayer*)i)->sendNetworkPacket(trpk);
	}
	return true;
}

static bool onReload(CommandOrigin const& ori, CommandOutput& outp) {
	loadCfg();
	loadCfgtpa();
	outp.success(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("hreload.success"));
	return true;
}
/*
bool oncmd_setPlNum(CommandOrigin const& ori, CommandOutput& outp, int num,int num1) {
		FAKE_ONLINEPLAYER = num;
		MAXONLINEPLAYER = num1;
		setServerMotd();
		if (num >= num1) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r "+ _TRS("setPlNum.set.error"));
			return false;
		}
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r "+ _TRS("setPlNum.success"));
		return true;
}
bool oncmd_setPlNumop(CommandOrigin const& ori, CommandOutput& outp, int num, int num1) {
	FAKE_ONLINEPLAYER = num;
	MAXONLINEPLAYER = num1;
	setServerMotd();
	outp.success(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("setPlNum.success"));
	return true;
}

bool oncmd_TPS(CommandOrigin const& ori, CommandOutput& outp) {
	float total = 0;
	auto size = tps.size();
	while (!tps.empty()) {
		total += tps.front();
		tps.pop();
	}
	outp.success(u8"§l§6[§eMCBE §bFINE§6]§rTPS:"+std::to_string(total / size));
	return true;
}
*/
bool oncmd_Lore(CommandOrigin const& ori, CommandOutput& outp, string& lore) {
	auto pl = MakeSP(ori);
	if (pl) {
		vector<string> l;
		split(lore, '_', l);
		setItemLore((Player*)pl, l);
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("setlore.success"));
	}
	return false;
}

std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	//std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
	return timestamp;
}
map<string, long > mcmc;
std::map < string, long > ::reverse_iterator itera;
#include<llmoney.h>
#include <regex>
class PathNavigation;
bool onCMD_TPR(CommandOrigin const& ori, CommandOutput& outp) {
	WPlayer wp = MakeWP(ori).val();
	if (wp->getDimensionId() != 0) {
		sendText(wp, u8"§l§6[§eMCBE §bFINE§6]§r 只能在主世界使用哦！", RAW);
		return false;
	}
	long ab = getTimeStamp();
	ServerPlayer* sp = MakeSP(ori);
	auto playername = sp->getNameTag();
	for (itera = mcmc.rbegin(); itera != mcmc.rend(); itera++)
		if (itera->first == playername) {
			if (ab - mcmc[playername] <= (long)1800000) {
				sendText(sp, u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tpr.fail.rate"), RAW);
				return false;
			}
		}
	if (Money::getMoney(offPlayer::getXUID(sp)) <100) {
		sendText(sp, u8"§l§6[§eMCBE §bFINE§6]§r 你没用足够的猫粮来随机传送", RAW);
		return false;
	}
	Money::createTrans(offPlayer::getXUID(sp), 0, 100,"tpr");
	srand((unsigned)time(NULL) * 1000);
	srand((unsigned)time(NULL) * 1000);
	mcmc[playername] = ab;
	mcmc.insert(std::map < string, long > ::value_type(playername, ab));
	Mob* mobs = (Mob*)sp;
	auto pxx = -2*rand() % (TPRNUM * 2 + 1) + TPRNUM/2;
	auto pzz = -2*rand() % (TPRNUM * 2 + 1) + TPRNUM/2;
	Vec3 posa = { pxx , 500 , pzz };
	wp.teleport(posa, wp.getDimID());
	sendText(sp, u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tpr.fail.inreq"), RAW);
	std::thread t([mobs, sp, pxx, pzz] {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		PathNavigation* v9 = (PathNavigation*)*((uintptr_t*)mobs + 44);
		BlockSource* v10 = (struct BlockSource*)*((uintptr_t*)mobs + 109);
		auto v26 = *(Vec2*)((char*)mobs + 1260);
		for (int i = 256; i > 0; i--) {
			Vec3 pos = {pxx, i ,pzz};
			bool hy = SymCall("?_isPositionOnlyInAir@PathNavigation@@IEBA_NAEBVBlockSource@@AEBVVec3@@AEBVVec2@@@Z"
				, bool, PathNavigation*, BlockSource*, Vec3&, Vec2&)(v9, v10, pos, v26);			
			if (hy == 0) {
				auto wpp = *(WPlayer*)mobs;
				Vec3 posab = { pxx , i + 1 , pzz };
				//SymCall("?teleport@TeleportCommand@@SAXAEAVActor@@VVec3@@PEAV3@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z", void, Actor*, Vec3, Vec3*, int)(mobs, posab, &posab, mobs->getDimensionId());
				char mem[128];
				SymCall("?computeTarget@TeleportCommand@@SA?AVTeleportTarget@@AEAVActor@@VVec3@@PEAV4@V?$AutomaticID@VDimension@@H@@VRelativeFloat@@4H@Z", void*, void*, Actor*, Vec3, int, int, float, float, int) (
					&mem, mobs, posab, 0, mobs->getDimensionId(), 0, 0, 15
					);
				SymCall("?applyTarget@TeleportCommand@@SAXAEAVActor@@VTeleportTarget@@@Z", void, Actor*, void*) (
					mobs, &mem
					);
				sendText(sp, u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tpr.success") + " X:" + to_string(pxx) + " Y:" + to_string(i + 1) + " Z:" + to_string(pzz)+u8", 共花费100猫粮", RAW);
				break;
			}
		}
		});
	t.detach();

	return true;
}
enum class JQZQOP :int {
	open = 1,
	close = 2,
};
unordered_map<string, int> jzqz;
bool oncmd_jzqz(CommandOrigin const& ori, CommandOutput& outp, MyEnum<JQZQOP> op, optional<int> num) {
	auto pl = MakeSP(ori);
	if (pl) {
		switch (op.val)
		{
		case JQZQOP::open: {
			if (!num.set) {
				outp.error(u8"§l§6[§eMCBE §bFINE§6]§r 你没有设置放置数(需要为奇数)");
				return true;
			}
			if (num.val() % 2 == 0) {
				outp.error(u8"§l§6[§eMCBE §bFINE§6]§r 该值必须为奇数");
				return true;
			}
			else {
				if (num.val() > 0 && num.val() <= 20) {
					jzqz[offPlayer::getRealName(pl)] = num.val();
					outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 设置成功,放置数为" + to_string(num.val()));
					return true;
				}
				else {
					outp.error(u8"§l§6[§eMCBE §bFINE§6]§r 超过设置区间，num∈(1~20]");
					return true;
				}
			}
			return true;
		}
		case JQZQOP::close: {
			if (num.set) {
				outp.error(u8"§l§6[§eMCBE §bFINE§6]§r 如果想要关闭，请不要设置放置数");
				return true;
			}
			if (jzqz[offPlayer::getRealName(pl)] == 114514) {
				outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 你并没有在建筑模式");
				return true;
			}
			jzqz[offPlayer::getRealName(pl)] = 114514;
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 关闭建筑模式成功");
			return true;
		}

		}
	}
	return false;
}

void REGCMD() {
	loadCNAME();
	Event::addEventListener([](RegCmdEV e) {
		CMDREG::SetCommandRegistry(e.CMDRg);
		CEnum<JQZQOP> _4("jzqz", { "open","close" });
		//MakeCommand("build", u8"建筑模式", 0);
		//CmdOverload(build, oncmd_jzqz, u8"选项", u8"放置数");
		if(ENABLERUNAS){
			MakeCommand("runas", "runas player", 1);
			CmdOverload(runas, onRunAS, "target", "command");
		}
		//if (ENABLETPS) {
		//	MakeCommand("tps", "show tps", 1);
		//	CmdOverload(tps, oncmd_TPS);
		//}
		if (ENABLELORE) {
			MakeCommand("lore", "set lore", 1);
			CmdOverload(lore, oncmd_Lore, "lore");
		}
		CEnum<BANOP> _1("banop", { "ban","unban","banip" ,"getdeviceid"});
		CEnum<BANOP_LIST> _2("banoplist", { "list" });
		CEnum<CNAMEOP> _3("cnameop", { "set","rm" });
		if (ENABLEGMODE) {
			MakeCommand("gmode", "set your gametype", 1);
			CmdOverload(gmode, oncmd_gmode, "target", "mode");
		}
		if (ENABLEBAN)
		{
			MakeCommand("ban", "blacklist", 1);
			CmdOverload(ban, onCMD_Ban, "op", "target", "time");
			CmdOverload(ban, onCMD_BanList, "list");
		}
		if (ENABLETRANSFER) {
			MakeCommand("transfer", "transfer player to another server", 1);
			CmdOverload(transfer, onCMD_Trans, "target", "host", "port");
		}
		MakeCommand("hreload", "reload cmdhelper", 1);
		CmdOverload(hreload, onReload);
		if (ENABLESKICK) {
			MakeCommand("skick", "force kick", 1);
			CmdOverload(skick, onCMD_skick, "target","reason");
		}
		if (ENABLECNAME) {
			MakeCommand("cname", "custom name", 1);
			CmdOverload(cname, onCMD_CNAME, "op", "target", "name");
		}
		if (ENABLEVANISH) {
			MakeCommand("vanish", "hide yourself", 1);
			CmdOverload(vanish, oncmd_vanish);
		}
		/*
		if (fakeonlineplayer) {
			MakeCommand("setplnum", "set fake online player num", 1);
			CmdOverload(setplnum, oncmd_setPlNum,"onlinenum","maxnum");
			MakeCommand("setplnum_op", "set fake online player num", 1);
			CmdOverload(setplnum_op, oncmd_setPlNumop, "onlinenum", "maxnum");
		}*/
		if (ENABLETPR) {
			MakeCommand("tpr", u8"随机传送", 0);
			CmdOverload(tpr, onCMD_TPR);
		}
		MakeCommand("reloadserver", u8"重启", 1);
		CmdOverload(reloadserver, oncmd_kickall);
		});
}
/*
THook(void, "?_onClientAuthenticated@ServerNetworkHandler@@AEAAXAEBVNetworkIdentifier@@AEBVCertificate@@@Z", void* snh, NetworkIdentifier& neti, Certificate& cert) {
	original(snh, neti, cert);
	auto xuid = ExtendedCertificate::getXuid(cert);
	auto be1 = getBanEntry(xuid);
	auto IP = liteloader::getIP(neti);
	auto be2 = getBanEntry(IP);
	auto nh = mc->getServerNetworkHandler();
	//LOG1 << "[" << gettime() << u8" INFO][BH] IP: " << liteloader::getIP(neti) << u8" 尝试与服务器连接" << endl;
	if (ENABLEBAN) {
		if (be1.set) {
			if (be1.val() != 0 && be1.val() < time(0)) {
				removeBanEntry(xuid);
			}
			else {
				nh->onDisconnect(neti);
			}
		}
		if (be2.set) {
			if (be2.val() != 0 && be2.val() < time(0)) {
				removeBanEntry(IP);
			}
			else {
				nh->onDisconnect(neti);
			}
		}
	}
}*/

namespace views {
	enum FACING {
		NEG_Y = 0,
		POS_Y = 1,
		NEG_Z = 2,
		POS_Z = 3,
		NEG_X = 4,
		POS_X = 5,
	};


	std::string facingToDirString(FACING facing) {
		switch (facing) {
		case FACING::POS_X:
			return "west";
		case FACING::NEG_X:
			return "east";
		case FACING::POS_Y:
			return "up";
		case FACING::POS_Z:
			return "south";
		case FACING::NEG_Y:
			return "down";
		case FACING::NEG_Z:
			return "north";
		default:
			return "unknown";
		}
	}

	std::string toDirString(Vec3 a1) {
		FACING fx = a1.x > 0 ? FACING::POS_X : FACING::NEG_X;
		FACING fz = a1.z > 0 ? FACING::POS_Z : FACING::NEG_Z;
		std::string s;
		if (abs(a1.x) >= abs(a1.z)) {
			s = facingToDirString(fx);
		}
		else if (1.732 * abs(a1.z) > abs(a1.x)) {
			s = facingToDirString(fz);
		}
		return s;
	}
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
	int hasitem(string name, string blockname, string aux) {
		auto out = liteloader::runcmdEx("clear \"" + name + "\" " + blockname + " " + aux + " 0");
		if (!out.first) {
			return 0;
		}		
		int nums = 0;
		std::regex express("[a-zA-Z_0-9 ]* has ([0-9]{0,4}) items that match the criteria");
		std::match_results<std::string::iterator> results1;
		if (std::regex_match(out.second.begin(), out.second.end(), results1, express)) {
			std::match_results<std::string::iterator>::const_iterator iter;
			for (iter = results1.begin(); iter != results1.end(); iter++)
			{
				nums = atoi(iter->str().c_str());
			}
		}
		return nums;
	}
	static int jzqz_DISTANCE_LAND = -2;
	bool setblock(Block* bl, BlockPos blpos, BlockSource* bs, ServerPlayer* pl, string blockname, string aux, string vie) {
		auto num = jzqz[offPlayer::getRealName(pl)];
		auto itemnum = hasitem(offPlayer::getRealName(pl), blockname, aux);
		int k = 0;
		int bx = blpos.x;
		int by = blpos.y;
		int bz = blpos.z;
		auto wp = WPlayer{ *pl };
		if (itemnum > num) {
			int ass = (num / 2);
			if (vie == "west" || vie == "east") {
				for (int p = 0; p < num; p++) {
					auto startVc = IVec2{ bx , bz - 2 + p };
					auto endVc = IVec2{ bx , bz - 2 + p };
					startVc += -jzqz_DISTANCE_LAND;
					endVc += jzqz_DISTANCE_LAND;
					if (!checkLandOwnerRange_stub(startVc, endVc, pl->getDimensionId(), offPlayer::getXUID(pl))) {
						wp.sendText(u8"§l§c靠近他人领地，操作被拒绝" + blockname, TIP);
						return false;
					}
				}
				string cmd = "execute \"" + offPlayer::getRealName(pl) + "\" ~ ~ ~ fill " + to_string(bx) + " " + to_string(by) + " " + to_string(bz - ass) + " " + to_string(bx) + " " + to_string(by) + " " + to_string(bz + ass) + " " + blockname + " " + aux + " keep";
				auto in = liteloader::runcmdEx(cmd);
				std::string output = std::regex_replace(in.second, std::regex("[^0-9]*([0-9]+)"), std::string("$1"));
				auto nums = atoi(output.c_str());
				liteloader::runcmdEx("clear \"" + offPlayer::getRealName(pl) + "\" " + blockname + " " + aux + " " + to_string(nums));
			}
			else if (vie == "south" || vie == "north") {
				for (int p = 0; p < num; p++) {
					auto startVc = IVec2{ bx , bz - 2 + p };
					auto endVc = IVec2{ bx , bz - 2 + p };
					startVc += -jzqz_DISTANCE_LAND;
					endVc += jzqz_DISTANCE_LAND;
					if (!checkLandOwnerRange_stub(startVc, endVc, pl->getDimensionId(), offPlayer::getXUID(pl))) {
						wp.sendText(u8"§l§c靠近他人领地，操作被拒绝" + blockname, TIP);
						return false;
					}
				}
				string cmd = "execute \"" + offPlayer::getRealName(pl) + "\" ~ ~ ~ fill " + to_string(bx - ass) + " " + to_string(by) + " " + to_string(bz) + " " + to_string(bx + ass) + " " + to_string(by) + " " + to_string(bz) + " " + blockname + " " + aux + " keep";
				auto in = liteloader::runcmdEx(cmd);
				std::string output = std::regex_replace(in.second, std::regex("[^0-9]*([0-9]+)"), std::string("$1"));
				auto nums = atoi(output.c_str());
				liteloader::runcmdEx("clear \"" + offPlayer::getRealName(pl) + "\" " + blockname + " " + aux + " " + to_string(nums));
			}
			return true;
		}
		else {
			wp.sendText(u8"§l§6你没有足够的" + blockname + " 至少需要" + to_string(num) + "个", TIP);
			return false;
		}
	}
}

THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* bs, Block* blk, BlockPos blkpos, unsigned char a4, Actor* ac, bool a5) {
	auto pl = MakeSP(ac);
	if (pl) {
		if (jzqz[offPlayer::getRealName(pl)] == 114514) {
			return original(bs, blk, blkpos, a4, ac, a5);
		}
		auto item = pl->getSelectedItem();
		WPlayer wp = WPlayer{ *pl };
		if (!item.isNull()) {
			auto blanme = offBlock::getFullName(blk);
			string itemname = "";
			SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(item.getItem(), &itemname);
			if (blanme == itemname) {
				Vec3 view = pl->getViewVector(1);
				string fx = views::toDirString(view);
				auto aux = to_string(item.getAuxValue());
				switch (views::H(fx)) {
				case views::H("west"): {
					if (!views::setblock(blk, blkpos, bs, pl, blanme, aux, "west"))return false;
					return original(bs, blk, blkpos, a4, ac, a5);
				}
				case views::H("east"): {
					if (!views::setblock(blk, blkpos, bs, pl, blanme, aux, "east"))return false;
					return original(bs, blk, blkpos, a4, ac, a5);
				}
				case views::H("south"): {
					if (!views::setblock(blk, blkpos, bs, pl, blanme, aux, "south"))return false;
					return original(bs, blk, blkpos, a4, ac, a5);
				}
				case views::H("north"): {
					if (!views::setblock(blk, blkpos, bs, pl, blanme, aux, "north"))return false;
					return original(bs, blk, blkpos, a4, ac, a5);
				}
				case views::H("up"): {
					wp.sendText(u8"暂不支持向上");
					break;
				}
				case views::H("down"): {
					wp.sendText(u8"暂不支持向下");
					break;
				}
				case views::H("unknown"): {
					wp.sendText(u8"未知方向，请重试");
					break;
				}
				}
			}
			else {
				wp.sendText(u8"数据出错，请重试");
			}
		}
	}
}
/*
short tickss = 0;
THook(void, "?normalTick@Player@@UEAAXXZ", Player* pl) {
	tickss++;
	if (tickss == 20) {
		if (jzqz[offPlayer::getRealName(pl)] == 114514) {
			tickss = 0;
			return original(pl);
		}
		WPlayer wp = WPlayer(*pl);
		auto item = pl->getSelectedItem();
		string mstr2 = "";
		short itemnum = 0;
		if (!item.getId() == 0) {
			SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
				void, const Item*, string*)(item.getItem(), &mstr2);

			itemnum = views::hasitem(offPlayer::getRealName(pl), mstr2, to_string(item.getAuxValue()));
		}
		else {
			mstr2 = "minecraft:hand";
		}
		wp.sendText(u8"§l§g目前处于建筑模式，手持物品: " + mstr2 + u8" 剩余：" + to_string(itemnum) + u8" 放置数：" + to_string(jzqz[offPlayer::getRealName(pl)]), JUKEBOX_POPUP);
		tickss = 0;
	}
	return original(pl);
}*/
THook(void, "?sendLoginMessageLocal@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@AEBVConnectionRequest@@AEAVServerPlayer@@@Z",
	ServerNetworkHandler* a1, NetworkIdentifier* a2, struct ConnectionRequest* a3, ServerPlayer* a4) {
	string mstr3 = "";
	string mstr2 = "";
	SymCall("?getDeviceId@ConnectionRequest@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, ConnectionRequest*, string*)(a3, &mstr3);
	auto out = SymCall("?getDeviceOS@ConnectionRequest@@QEBA?AW4BuildPlatform@@XZ", int, ConnectionRequest*, string*)(a3, &mstr2);
	LOG1 << u8"玩家：" << a4->getNameTag() << u8" 的DeviceId：" << mstr3 << u8" 的DeviceOS：" << out << endl;
	auto be1 = getBanEntry(offPlayer::getXUIDString(a4));
	auto IP = liteloader::getIP(*a2);
	auto be2 = getBanEntry(IP);
	auto nh = mc->getServerNetworkHandler();
	auto be3 = getBanEntry(mstr3);
	//LOG1 << "[" << gettime() << u8" INFO][BH] IP: " << liteloader::getIP(neti) << u8" 尝试与服务器连接" << endl;
	if (ENABLEBAN) {
		if (be1.set) {
			if (be1.val() != 0 && be1.val() < time(0)) {
				removeBanEntry(offPlayer::getXUIDString(a4));
			}
			else {
				nh->disconnectClient(*a2, u8"你已经被封禁", 0);
			}
		}
		if (be2.set) {
			if (be2.val() != 0 && be2.val() < time(0)) {
				removeBanEntry(IP);
			}
			else {
				nh->disconnectClient(*a2, u8"你已经被封禁", 0);
			}
		}
		if (be3.set) {
			if (be2.val() != 0 && be2.val() < time(0)) {
				removeBanEntry(mstr3);
			}
			else {
				nh->disconnectClient(*a2, u8"你已经被封禁", 0);
			}
		}
	}
	return original(a1, a2, a3, a4);
}
