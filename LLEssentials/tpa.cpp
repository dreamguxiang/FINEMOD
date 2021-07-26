﻿// BDXTPA.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include <Llland.h>
#include "homeStorage.h"
#include "tpa.h"
#include"Helper.h"
bool checkLandOwnerRange_stub(IVec2 vc, IVec2 vc2, int dim, unsigned long long xuid);
using namespace std;
static LangPack LangP("plugins/LLEssentials/langpack/zh_cn.json");
std::unique_ptr<KVDBImpl> dbtpa;
static Logger LOG(stdio_commit{ "[TPA] " });
#pragma region structs

U64LRUmap<Homes, 17> HomeCache(48);
Homes& getHomeInCache(xuid_t xid) {
	Homes* rv = HomeCache.find(xid);
	if (!rv) {
		rv = HomeCache.insert(xid, xid);
	}
	return *rv;
}
enum direction :int {
	A_B = 1,
	B_A = 2
};
struct TPReq {
	direction dir;
	string A, B;
	clock_t time;
	TPReq() {}
	TPReq(direction a, string_view b, string_view c, clock_t d) :dir(a), A(b), B(c), time(d) {}
};
struct TPASet {
	clock_t lastReq = 0;
	bool Accept = true;
};


#pragma endregion
#pragma region gvals
#include<unordered_map>
using std::unordered_map;
static std::list<TPReq> reqs;
static unordered_map<CHash, TPASet> tpaSetting;
static unordered_map<string, Vec4> warps;

static clock_t TPexpire = CLOCKS_PER_SEC * 10;
static clock_t TPratelimit = CLOCKS_PER_SEC * 2;
static int MAX_HOMES = 5;
static int HOME_DISTANCE_LAND = 0;
static bool BACK_ENABLED, SUICIDE_ENABLED, TPA_ENABLED, HOME_ENABLED;


playerMap<Vec4> deathPos;
#pragma endregion

#pragma region TPA
optional<decltype(reqs.begin())> DoFetchReq(string_view a) {
	for (auto it = reqs.begin(); it != reqs.end(); ++it) {
		if (it->A == a || it->B == a) return { it };
	}
	return {};
}
optional<decltype(reqs.begin())> DoFetchReq_sender(string_view a) {
	auto rv = DoFetchReq(a);
	if (rv.set && rv.value()->A == a) return rv;
	return {};
}
optional<decltype(reqs.begin())> DoFetchReq_receiver(string_view a) {
	auto rv = DoFetchReq(a);
	if (rv.set && rv.value()->B == a) return rv;
	return {};
}
enum class TPFailReason :int {
	success = 0,
	ratelimit = 1,
	inreq = 2,
	blocked = 3
};
TPFailReason CanMakeReq(string_view a, string_view b) {
	CHash A = do_hash2(a), B = do_hash2(b);
	if (tpaSetting[A].lastReq >= clock() - TPratelimit) {
		return TPFailReason::ratelimit;
	}
	if (DoFetchReq(a).set || DoFetchReq(b).set) { return TPFailReason::inreq; }
	if (!tpaSetting[B].Accept) { return TPFailReason::blocked; }
	return TPFailReason::success;
}
enum class TPCloseReason :int {
	timeout = 1,
	deny = 2,
	accept = 3,
	cancel = 4
};



bool DoCloseReq(decltype(reqs.begin()) rq, TPCloseReason res) {
	string playernamea = rq->A;
	string playernameb = rq->B;
	vector<Player*> plist = liteloader::getAllPlayers();
	Player* fplA = nullptr;
	Player* fplB = nullptr;
	for (auto p : plist) {
		if (offPlayer::getRealName(p) == playernamea) {
			fplA = p;
			break;
		}
	}
	for (auto p : plist) {
		if (offPlayer::getRealName(p) == playernameb) {
			fplB = p;
			break;
		}
	}
	if (fplA && fplB) {
		optional<WPlayer> A = WPlayer(*(ServerPlayer*)fplA);
		optional<WPlayer> B = WPlayer(*(ServerPlayer*)fplB);
		if (res == TPCloseReason::cancel) {
			reqs.erase(rq);
			return true;
		}
		if (res == TPCloseReason::accept) {
			if (A.set && B.set) {
				Vec4 AP{ (rq->dir == A_B ? B : A).value() };
				AP.teleport((rq->dir == A_B ? A : B).value());
				reqs.erase(rq);
				A.value().sendText(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("tpa.reason.accept"));
				return true;
			}
			reqs.erase(rq);
			return false;
		}
		if (A.set) {
			A.value().sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + (res == TPCloseReason::deny ? _TRS("tpa.reason.deny") : _TRS("tpa.reason.timeout")));
		}
		if (B.set) {
			B.value().sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + (res == TPCloseReason::deny ? _TRS("tpa.reason.deny") : _TRS("tpa.reason.timeout")));
		}
		reqs.erase(rq);
		return true;
	}
	else {
		//no such player
	}
}
void DoMakeReq(WPlayer _a, WPlayer _b, direction dir) {
	auto a = offPlayer::getRealName(_a);
	auto b = offPlayer::getRealName(_b);
	CHash A = do_hash2(a), B = do_hash2(b);
	tpaSetting[A].lastReq = clock();
	reqs.emplace_back(dir, a, b, clock());
	string prompt = a + (dir == A_B ? _TRS("tpa.req.A_B") : _TRS("tpa.req.B_A"));
	_b.sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + prompt );
	using namespace GUI;
	shared_ptr<RawFormBinder> x;
	char buf[1024];
	string FM{ buf,(size_t)snprintf(buf,1024,_TR("tpa.form"), prompt.c_str()) };
	sendForm(_b, RawFormBinder{ FM,[](WPlayer wp,RawFormBinder::DType i) {
		auto [clicked,res,list] = i;
		if (clicked) {
			int idx = atoi(res);
			wp.runcmdA("tpa",(idx == 0 ? "ac" : "de"));
		}
	} ,{} });
}

#include <api/scheduler/scheduler.h>
void schTask() {
	Handler::schedule(RepeatingTask([] {
		clock_t expire = clock() - TPexpire;
		for (auto it = reqs.begin(); it != reqs.end();) {
			if (it->time <= expire) {
				auto oldit = it;
				++it;
				DoCloseReq(oldit, TPCloseReason::timeout);
			}
			else break;
		}
		}, 10)); //5sec
}

bool oncmd_tpa(CommandOrigin const& ori, CommandOutput& outp, MyEnum<direction> dir, CommandSelector<Player>& target) {
	auto res = target.results(ori);
	if (!Command::checkHasTargets(res, outp) || (res.count() != 1)) return false;
	WPlayer t{ **res.begin() };
	auto reqres = CanMakeReq(ori.getName(), offPlayer::getRealName(t));
	switch (reqres) {
	case TPFailReason::success:
	{
		DoMakeReq({ *(ServerPlayer*)ori.getEntity() }, t, dir);
		return true;
	}
	break;
	case TPFailReason::ratelimit: {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tp.fail.rate"));
		return false;
	}
								break;
	case TPFailReason::inreq: {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tp.fail.inreq"));
		return false;
	}
							break;
	case TPFailReason::blocked: {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("tp.fail.blocked"));
		return false;
	}
							  break;
	}
	return false;
}
enum class TPAOP :int {
	ac = 1, de = 2, cancel = 3, toggle = 4,gui = 5
};


std::vector<string> playerlist() {
	auto plist = liteloader::getAllPlayers();
	std::vector<string> pl;
	for (auto p : plist) {
		pl.push_back(p->getNameTag());
	}
	return pl;
}
bool oncmd_tpa2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<TPAOP> op) {
	switch (op) {
	case TPAOP::ac: {
		auto it = DoFetchReq_receiver(ori.getName());
		if (!it.set) {
			return false;
		}
		DoCloseReq(it.val(), TPCloseReason::accept);
		break;
	}
	case TPAOP::de: {
		auto it = DoFetchReq_receiver(ori.getName());
		if (!it.set) {
			return false;
		}
		DoCloseReq(it.val(), TPCloseReason::deny);
		break;
	}
	case TPAOP::cancel: {
		auto it = DoFetchReq_sender(ori.getName());
		if (!it.set) {
			return false;
		}
		DoCloseReq(it.val(), TPCloseReason::cancel);

		break;
	}
	case TPAOP::toggle: {
		CHash hs = do_hash2(ori.getName());
		auto state = !tpaSetting[hs].Accept;
		tpaSetting[hs].Accept = state;
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r new state " + std::to_string(state));

		break;
	}
	case TPAOP::gui: {
		auto _wp = MakeWP(ori);
		auto wp = _wp.val();
		using namespace GUI;
		auto fm = std::make_shared<FullForm>();
		fm->title = u8"TPA 快捷菜单";
		fm->addWidget({ GUILabel("HI " + wp->getNameTag())});
		fm->addWidget({ GUIDropdown(u8"过去还是到你这",{"to","here"}) });
		fm->addWidget({ GUIDropdown("Players",playerlist()) });
		sendForm(wp, FullFormBinder{ fm,{[](WPlayer P, FullFormBinder::DType data) {
			if (!data.set) return;
				auto& [d1,d2] = data.val();
				liteloader::runcmdAs(P, "tpa " + d2[0] +" "+ QUOTE(d2[1]));
		}} });

		break;
	}
	}
	return true;
}
#pragma endregion

#pragma region WARP

shared_ptr<GUI::SimpleForm> WARPGUI;
void reinitWARPGUI() {
	using namespace GUI;
	if (!WARPGUI) WARPGUI = make_shared<SimpleForm>();
	WARPGUI->title = u8"Warp 菜单";
	WARPGUI->content = u8"选择一个传送点";
	WARPGUI->reset();
	for (auto& [k, v] : warps) {
		WARPGUI->addButton(GUIButton(string(k)));
	}
}
void sendWARPGUI(WPlayer wp) {
	using namespace GUI;
	sendForm(wp, SimpleFormBinder(WARPGUI, [](WPlayer wp, SimpleFormBinder::DType d) {
		if (d.set) {
			wp.runcmdA("warp", "go", QUOTE(d.val().second));
		}
		}));
}

void saveWarps() {
	WBStream ws;
	ws.apply(warps);
	dbtpa->put("warps", ws);
	reinitWARPGUI();
}
enum WARPOP :int {
	go = 1, add = 2, ls = 3, del = 4, gui = 5
};
enum class HOMEOP :int {
	go = 1, add = 2, ls = 3, del = 4
};

bool oncmd_warp(CommandOrigin const& ori, CommandOutput& outp, MyEnum<WARPOP> op, optional<string>& val) {
	switch (op)
	{
	case gui: {
		sendWARPGUI(MakeWP(ori).val());
		return true;
		break;
	}
	case add: {
		if (ori.getPermissionsLevel() < 1) return false;
		WActor wa{ MakeWP(ori).val() };
		warps.emplace(val.value(), wa);
		saveWarps();
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("warp.add.success"));
		break;
	}
	case del: {
		if (ori.getPermissionsLevel() < 1) return false;
		if (warps.erase(val.value())) {
			saveWarps();
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("warp.del.success"));
		}
		else
		{
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 传送点 `" + val.value() + u8" 没有被找到!");
		}
		break;
	}
	case ls: {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 传送点列表");
		for (auto& i : warps) {
			outp.addMessage(i.first + " " + i.second.toStr());
		}

		break;
	}
	case go: {
		auto it = warps.find(val.value());
		if (it == warps.end()) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("home.not.found"));
			return false;
		}
		it->second.teleport(MakeWP(ori).val());

		break;
	}
	default:
		break;
	}
	return true;
}
#pragma endregion
#pragma region HOME
bool generic_home(CommandOrigin const& ori, CommandOutput& outp, Homes& hm, MyEnum<HOMEOP> op, optional<string>& val) {
	switch (op)
	{
	case HOMEOP::add: {
		if (ori.getPermissionsLevel() == 0 && hm.data.size() >= MAX_HOMES) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("home.is.full"));
			return false;
		}
		WPlayer wp = MakeWP(ori).val();
		Vec4 vc{ wp };
		IVec2 startVc{ wp->getPos() };
		IVec2 endVc{ wp->getPos() };
		startVc += -HOME_DISTANCE_LAND;
		endVc += HOME_DISTANCE_LAND;
		if (!checkLandOwnerRange_stub(startVc, endVc, vc.dimid, wp.getXuid())) {
			outp.error(_TRS("home.near.others.land"));
			return false;
		}
		hm.data.emplace_back(val.value(), vc);
			hm.save();
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 家园 " + val.value() + u8" 设置成功!");

			break;
	}
	case HOMEOP::del: {
		for (auto& i : hm.data) {
			if (i.name == val.value()) {
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 家园 " + val.value() + u8" 删除成功!");
				hm.data.remove_if([&](auto& x) {
					return x.name == val.value();
					});
				hm.save();
				return false;
			}
		}
		outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 家园 " + val.value() + " 没有被找到!");
		hm.save();
		break;
	}
	case HOMEOP::ls: {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 家园列表");
		for (auto& i : hm.data) {
			outp.addMessage(i.name + " " + i.pos.toStr());
		}

		break;
	}
	case HOMEOP::go: {
		for (auto& i : hm.data) {
			if (i.name == val.value()) {
				i.pos.teleport(MakeWP(ori).val());
				return true;
			}
		}
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("home.not.found"));
		return false;
		break;
	}
	default:
		break;
	}
	return true;
}
static string api[] = {
	"add&del home",
	"back home"

};

enum class HOMEgui :int {
	gui = 1
};
bool homegui(CommandOrigin const& ori, CommandOutput& outp, Homes& hm, MyEnum<HOMEgui> op) {
	auto wp = MakeWP(ori).val();
	switch (op)
	{
	case HOMEgui::gui: {
		using namespace GUI;
		auto mainhomegui = make_shared<SimpleForm>();
		mainhomegui->title = u8"家园菜单";
		mainhomegui->content = u8"HI " + wp->getNameTag();
		mainhomegui->reset();
		for (auto& i : api) {
			mainhomegui->addButton(GUIButton(string(i)));
		}
		sendForm(wp, SimpleFormBinder(mainhomegui, [hm](WPlayer wp, SimpleFormBinder::DType d1) {
			if (d1.set) {
				if (d1.val().first == 0) {
					auto fm = std::make_shared<FullForm>();
					fm->title = u8"删除还是添加家园";
					fm->addWidget({ GUILabel("HI " + wp->getNameTag()) });
					fm->addWidget({ GUIDropdown(u8"添加还是删除",{"add","del"}) });
					fm->addWidget({ GUIInput(u8"你家园的名字？ ") });
					sendForm(wp, FullFormBinder{ fm,{[](WPlayer P, FullFormBinder::DType data) {
						if (!data.set) return;
							auto& [d1,d2] = data.val();
							P.runcmdA("home", d2[0], std::get<string>(d1[2]));
					}} });
				}
				if (d1.val().first == 1) {
					auto HomeGUI = make_shared<SimpleForm>();
					HomeGUI->title = u8"家园传送菜单";
					HomeGUI->content = u8"选择一个家";
					HomeGUI->reset();
					for (auto& i : hm.data) {
							HomeGUI->addButton(GUIButton(string(i.name)));
					}
					sendForm(wp, SimpleFormBinder(HomeGUI, [](WPlayer wp, SimpleFormBinder::DType d) {
						if (d.set) {
							wp.runcmdA("home", "go", QUOTE(d.val().second));
						}
						}));
			    }
			}
			}));
		break;
	}
	default:
		break;
	}
	return true;
}

bool oncmd_home(CommandOrigin const& ori, CommandOutput& outp, MyEnum<HOMEOP> op, optional<string>& val) {
	return generic_home(ori, outp, getHomeInCache(offPlayer::getXUID(MakeWP(ori).val())), op, val);
}
bool oncmd_homeAs(CommandOrigin const& ori, CommandOutput& outp, string const& target, MyEnum<HOMEOP> op, optional<string>& val) {
	return generic_home(ori, outp, getHomeInCache(XIDREG::str2id(target).val()), op, val);
}
bool oncmd_homegui(CommandOrigin const& ori, CommandOutput& outp, MyEnum<HOMEgui> op) {
	return homegui(ori, outp, getHomeInCache(offPlayer::getXUID(MakeWP(ori).val())), op);
}
#pragma endregion
#pragma region BACK
bool oncmd_back(CommandOrigin const& ori, CommandOutput& outp) {
	ServerPlayer* sp = (ServerPlayer*)ori.getEntity();
	if (!deathPos._map.count(sp)) {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r "+_TRS("home.not.found"));
		return false;
	}
	deathPos[sp].teleport({ *sp });
	deathPos._map.erase(sp);

	return true;
}
#pragma endregion
bool oncmd_suicide(CommandOrigin const& ori, CommandOutput& outp) {
	auto wp = MakeWP(ori);
	if (wp.set) {
		SymCall("?kill@Mob@@UEAAXXZ", void, Mob*)(wp.val());
		return true;
	}
	return true;
}
void loadCfgtpa() {
	try {
		ConfigJReader jr("plugins/LLEssentials/tpa.json");
		jr.bind("max_homes", MAX_HOMES, 5);
		jr.bind("tpa_timeout", TPexpire, CLOCKS_PER_SEC * 20);
		jr.bind("tpa_ratelimit", TPratelimit, CLOCKS_PER_SEC * 5);
		jr.bind("home_land_distance", HOME_DISTANCE_LAND, -1);
		jr.bind("BACK_ENABLED", BACK_ENABLED, true);
		jr.bind("SUICIDE_ENABLED", SUICIDE_ENABLED, true);
		jr.bind("TPA_ENABLED", TPA_ENABLED, true);
		jr.bind("HOME_ENABLED", HOME_ENABLED, true);
	}
	catch (string e) {
		LOG("[LLEssentials]JSON ERROR", e);
		throw 0;
	}
}
void loadall() {
	string val;
	dbtpa = MakeKVDB(GetDataPath("tpa"), true, 8);
	if (dbtpa->get("warps", val)) {
		RBStream rs{ val };
		rs.apply(warps);
	}
	loadCfgtpa();
}
void tpa_entry() {
	loadall();
	reinitWARPGUI();
	checkLandOwnerRange_stub({ 0, 0 }, { 0, 0 }, 0, 0);
	schTask();
	Event::addEventListener([](RegCmdEV e) {
		CMDREG::SetCommandRegistry(e.CMDRg);
		CEnum<direction> _1("tpdir", { "to","here" });
		CEnum<WARPOP> _2("warpop", { "go","add","ls","del","gui" });
		CEnum<HOMEOP> _4("homeop", { "go","add","ls","del" });
		CEnum<TPAOP> _3("tpaop", { "ac","de","cancel","toggle","gui" });
		CEnum<HOMEgui> _5("homeopop", { "gui" });
		if (TPA_ENABLED) {
			MakeCommand("tpa", "tpa system", 0);
			CmdOverload(tpa, oncmd_tpa, "dir", "target");
			CmdOverload(tpa, oncmd_tpa2, "op");
		}
		if (HOME_ENABLED) {
			MakeCommand("warp", "warp system", 0);
			MakeCommand("home", "home system", 0);
			CmdOverload(warp, oncmd_warp, "op", "name");
			CmdOverload(home, oncmd_home, "op", "name");
			CmdOverload(home, oncmd_homegui, "op");
		}
		if (BACK_ENABLED) {
			MakeCommand("back", "back to last deathpoint", 0);
			CmdOverload(back, oncmd_back);
		}
		if (SUICIDE_ENABLED) {
			MakeCommand("suicide", "kill yourself", 0);
			CmdOverload(suicide, oncmd_suicide);
		}
		});
	if (BACK_ENABLED) {
		Event::addEventListener([](PlayerDeathEV  ev) {
			auto sp = ev.Player;
			auto p = WPlayer(*(ServerPlayer*)sp);
			deathPos[p.v] = Vec4{ p };
			p.sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("tpa.back.use"),RAW);
			});
	}
}