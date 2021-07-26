#include "pch.h"
#include <loader/Loader.h>
#include <iostream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "mc/Player.h"
#include "mc/item.h"
#include <api/types/types.h>
#include <api/LiteLoaderApi.h>
#include <mc/BlockSource.h>
#include <mc/Block.h>
#include <mc/OffsetHelper.h>
#include<api\regCommandHelper.h>
#include<api/basicEvent.h>
#include <map>
#include "pch.h"
#include<fstream>
#include<streambuf>
#include<api/xuidreg/xuidreg.h>
#include <llmoney.h>
using namespace std;
double MoneyFee;
LangPack LangP("plugins\\LLMoney\\langpack\\money.json");
static Logger LOG(stdio_commit{ "[MONEY] " });
bool initDB();
void Version() {
	cout << "[LLMoney] version 210409" << endl;
}
extern money_t DEF_MONEY;
enum MONEYOP :int {
	query = 1,
	hist = 2,
};
enum MONEYOP_pay :int {
	pay = 1,
	set = 2,
	add = 3,
	reduce = 4
};
enum MONEYOP_PURGE :int {
	purge = 1
};
bool oncmd_money(CommandOrigin const& ori, CommandOutput& outp, MyEnum<MONEYOP> op, optional<string>& dst) {
	using namespace Money;
	optional<xuid_t> dstid;
	if (dst.set && ori.getPermissionsLevel() > 0) {
		dstid = XIDREG::str2id(dst.val());
	}
	else {
		dstid = XIDREG::str2id(ori.getName());
	}
	if (!dstid.Set()) {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.target"));
		return false;
	}
	switch (op)
	{
	case query:
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 你的猫粮: " + std::to_string(getMoney(dstid.val())));
		break;
	case hist:
		outp.addMessage(getTransHist(dstid.val()));
		break;
	}
	return true;
}

bool oncmd_money2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<MONEYOP_pay> op, string const& dst, int val) {
	using namespace Money;
	optional<xuid_t> dstxuid, myuid;
	dstxuid = XIDREG::str2id(dst);
	if (!dstxuid.Set()) {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.target"));
		return false;
	}
	switch (op)
	{
	case pay:
	{
		if (val <= 0) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.invalid.arg"));
			return false;
		}
		myuid = XIDREG::str2id(ori.getName());
		if (!myuid.Set()) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.target"));
			return false;
		}
		if (createTrans(myuid.val(), dstxuid.val(), val, "money pay")) {
			money_t fee = (money_t)(val * MoneyFee);
			if (fee)
				createTrans(dstxuid.val(), 0, fee, "money pay fee");
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 转账成功");
			return true;
		}
		else {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.not.enough"));
			return false;
		}
	}
	break;
	case set:
		if (ori.getPermissionsLevel() < 1) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.perm"));
			return false;
		}
		if (setMoney(dstxuid.val(), val)) {
			outp.success("set success");
			return true;
		}
		else {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.invalid.arg"));
			return false;
		}
		break;
	case add:
		if (ori.getPermissionsLevel() < 1) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.perm"));
			return false;
		}
		if (createTrans(0, dstxuid.val(), val, "money add")) {
			outp.success(u8"§l§6[§eMCBE §bFINE§6]§r 添加成功");
			return true;
		}
		else {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.invalid.arg"));
			return false;
		}
		break;
	case reduce:
		if (ori.getPermissionsLevel() < 1) {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.perm"));
			return false;
		}
		if (createTrans(dstxuid.val(), 0, val, "money reduce")) {
			return true;
		}
		else {
			outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.invalid.arg"));
			return false;
		}
		break;
	}
	return true;
}

bool oncmd_money3_p(CommandOrigin const& ori, CommandOutput& outp, MyEnum<MONEYOP_PURGE> op, optional<int>& difftime) {
	if (ori.getPermissionsLevel() < 1) {
		outp.error(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("money.no.perm"));
		return false;
	}
	if (difftime.Set())
		Money::purgeHist(difftime.val());
	else
		Money::purgeHist(0);
	return true;
}


void entry() {
	Version();
	filesystem::create_directory("plugins\\LLMoney");
	filesystem::create_directory("plugins\\LLMoney\\langpack");
	if (!initDB()) {
		exit(1);
	}
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("money", "money", 0);
		CEnum<MONEYOP> _1("type", { "query","hist" });
		CEnum<MONEYOP_pay> _2("type2", { "pay","set","add","reduce" });
		CEnum<MONEYOP_PURGE> _3("mpurge", { "purge" });
		CmdOverload(money, oncmd_money, "op", "target");
		CmdOverload(money, oncmd_money2, "op", "target1", "money");
		CmdOverload(money, oncmd_money3_p, "purge", "difftime");
		});
	try {
		ConfigJReader jr("plugins\\LLMoney\\money.json");
		int defmoney;
		jr.bind("def_money", defmoney, 0);
		jr.bind("pay_tax", MoneyFee, .0);
		DEF_MONEY = defmoney;
	}
	catch (string e) {
		LOG.p<LOGLVL::Error>("json error", e);
		throw 0;
	}
}
extern "C" {
	_declspec(dllexport) money_t llgetMoney(xuid_t);
	_declspec(dllexport) string llgetTransHist(xuid_t, int);
	_declspec(dllexport) bool llcreateTrans(xuid_t, xuid_t, money_t, string const&);
	_declspec(dllexport) bool llsetMoney(xuid_t, money_t);
	_declspec(dllexport) bool lladdMoney(xuid_t, money_t);
	_declspec(dllexport) bool llreduceMoney(xuid_t, money_t);
	_declspec(dllexport) void llpurgeHist(int);
}

money_t llgetMoney(xuid_t xuid)
{
	return Money::getMoney(xuid);
}
string llgetTransHist(xuid_t xuid, int timediff = 24 * 60 * 60)
{
	return  Money::getTransHist(xuid, timediff);
}
bool llcreateTrans(xuid_t from, xuid_t to, money_t val, string const& note = "")
{
	return  Money::createTrans(from, to, val, note);
}
bool llsetMoney(xuid_t xuid, money_t money)
{
	return  Money::setMoney(xuid, money);
}
bool lladdMoney(xuid_t xuid, money_t money)
{
	return  Money::createTrans(0, xuid, money, "money add");;
}
bool llreduceMoney(xuid_t xuid, money_t money)
{
	return  Money::reduceMoney(xuid, money);
}
void llpurgeHist(int difftime = 0)
{
	return  Money::purgeHist(difftime);
}