#include "pch.h"
using namespace std;
#pragma warning(disable:4996)
#include <api/scheduler/scheduler.h>
std::unique_ptr<KVDBImpl> db;

typedef std::uint64_t hash_t;

constexpr hash_t prime = 0x100000001B3ull;
constexpr hash_t basis = 0xCBF29CE484222325ull;

hash_t hash_(char const* str)
{
	hash_t ret{ basis };

	while (*str) {
		ret ^= *str;
		ret *= prime;
		str++;
	}

	return ret;
}

constexpr hash_t H(char const* str, hash_t last_value = basis)
{
	return *str ? H(str + 1, (*str ^ last_value) * prime) : last_value;
}

inline long gettime()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime= localtime(&rawtime);
	strftime(timestr, 20, "%Y%m%d", LocTime);
	return atoi(timestr);
}

inline string gettimes()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", LocTime);
	return string(timestr);
}
inline string getyear()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%Y", LocTime);
	return timestr;
}
inline string getmonth()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%m", LocTime);
	return timestr;
}
inline string getday()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%d", LocTime);
	return timestr;
}
#include <string>
int getlastday(string a1,string a2) {
	int a;
	if (a1 == "1") return 31;
	if (a1 == "2") {
		if (atoi(a2) % 4 == 0) {
			return 29;
		}
		else {
			return 28;
		}
	}
	if (a1 == "3") return 31;
	if (a1 == "4") return 30;
	if (a1 == "5") return 31;
	if (a1 == "6") return 30;
	if (a1 == "7") return 31;
	if (a1 == "8") return 31;
	if (a1 == "9") return 30;
	if (a1 == "10") return 31;
	if (a1 == "11") return 30;
	if (a1 == "12") return 31;
}

struct QianDao {
	string name;
	int year;
	int month;
	int day;
	int num;
	int lxqd;
	string lastsignin;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(name,year,month,day,num,lxqd, lastsignin);
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
		bs.apply(name, year, month, day, num, lxqd, lastsignin);
		string val;
	}

};
map<string, QianDao > qiandao;
#include <llmoney.h>
bool qiandaocg(WPlayer wp,string a1, decltype(qiandao.begin()) rq ) {
	if (a1 == "xuqian") {
		QianDao qd;
		auto qdsj = rq->second.lastsignin;
		qd.name = rq->second.name;
		qd.day = atoi(getday());
		qd.month = atoi(getmonth());
		qd.year = atoi(getyear());
		qd.lastsignin = gettimes();
		qd.num = rq->second.num + 1;
		qd.lxqd = rq->second.lxqd + 1;
		WBStream ws;
		ws.apply(qd);
		db->put("qiandao_" + rq->second.name, ws);
		qiandao[rq->second.name] = qd;
		long money = 90 + rq->second.lxqd * 10;
		if (money > 300) {
			money = 300;
		}
		Money::createTrans(0, offPlayer::getXUID(wp), money, "signin");
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§bHI,"+ rq->second.name +u8" §l§6签到成功~\n§l§a您已经连续签到:"+to_string(rq->second.lxqd)+u8"天\n§l§c累计"
			u8"签到:"+ to_string(rq->second.num) +u8"天\n§l§e上次签到时间:"+ qdsj+ u8"\n§l§g本次签到时间:"+rq->second.lastsignin + u8"\n§l§6本次签到奖励:"+to_string(money)+u8"猫粮");
	}
	if (a1 == "duanqian") {
		QianDao qd;
		auto qdsj = rq->second.lastsignin;
		qd.name = rq->second.name;
		qd.day = atoi(getday());
		qd.month = atoi(getmonth());
		qd.year = atoi(getyear());
		qd.lastsignin = gettimes();
		qd.num = rq->second.num + 1;
		qd.lxqd = 1;
		WBStream ws;
		ws.apply(qd);
		db->put("qiandao_" + rq->second.name, ws);
		qiandao[rq->second.name] = qd;
		long money = 100;
		Money::createTrans(0, offPlayer::getXUID(wp), money, "signin");
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§bHI," + rq->second.name + u8"§l§6签到成功，§l§4但您已经断签\n§l§a您已经连续签到:" + to_string(rq->second.lxqd) + u8"天\n§l§c累计"
			u8"签到:" + to_string(rq->second.num) + u8"天\n§l§e上次签到时间:" + qdsj + u8"\n§l§g本次签到时间:" + rq->second.lastsignin + u8"\n§l§6本次签到奖励:" + to_string(money) + u8"猫粮");
	}
	if (a1 == "buqian") {

	}
	if (a1 == "fail") {
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§c签到失败，您已经签到过了\n上次签到时间:"+ rq->second.lastsignin);
	}
}
bool oncmd_signin(CommandOrigin const& ori, CommandOutput& outp) {
	auto pl = MakeSP(ori);	
	auto wp = MakeWP(ori).val();
	auto plname = offPlayer::getRealName(pl);
	for (auto iter = qiandao.begin(); iter != qiandao.end(); iter++) {
		if (iter->first == plname) {
			if (iter->second.year != atoi(getyear())) {
				if (atoi(getyear()) - iter->second.year == 1) {
					if (iter->second.month == 12) {
						if (iter->second.day == 31) {
							if (getday() == "01") {
								qiandaocg(wp, "xuqian", iter);
								return true;
								//加一天
							}
						}
					}
				}
				qiandaocg(wp, "duanqian", iter);//断签
				return true;
			}
			else {
				if (iter->second.month != atoi(getmonth())) {
					if (atoi(getmonth()) - iter->second.month == 1) {
						auto lastday = getlastday(to_string(iter->second.month), to_string(iter->second.year));
						//cout << lastday <<endl;
						//cout << iter->second.day << endl;
						//cout << getday() << endl;
						if (iter->second.day == lastday) {
							if (getday() == "01") {
								qiandaocg(wp, "xuqian", iter);
								return true;
								//加一天
							}
						}
						qiandaocg(wp, "duanqian", iter);//断签
						return true;
					}
				}
				else
					if (iter->second.day != atoi(getday())) {
						if (atoi(getday()) - iter->second.day == 1) {
							qiandaocg(wp, "xuqian", iter);
							return true;
							//加一天
						}
						qiandaocg(wp, "duanqian", iter);//断签	
						return true;
					}
					else {
						qiandaocg(wp, "fail", iter);//已签
						return true;
					}
			}
		}
	}
	if (qiandao.find(offPlayer::getRealName(wp)) == qiandao.end()) {
		QianDao qd;
		qd.name = offPlayer::getRealName(wp);
		qd.day = atoi(getday());
		qd.month = atoi(getmonth());
		qd.year = atoi(getyear());
		qd.lastsignin = gettimes();
		qd.num = 1;
		qd.lxqd = 1;
		WBStream ws;
		ws.apply(qd);
		db->put("qiandao_" + offPlayer::getRealName(wp), ws);
		qiandao[offPlayer::getRealName(wp)] = qd;
		long money = 1000;
		Money::createTrans(0, offPlayer::getXUID(wp), money, "firstsignin");
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r §l§a签到成功，这是你来FINE第一次签到哦~");//首次签到
	}
	return true;
}

#include <FineSignIn.h>
namespace finesignin {
	FINESIGNIN_API std::vector<signinnumlist> getsigninnumlist() {
		std::vector<signinnumlist> res;
		for (auto& i : qiandao) {
			res.push_back({ i.first,i.second.num });
		}
		return res;
	}
	FINESIGNIN_API std::vector<signinlxqdlist> getsigninlxqdlist() {
		std::vector<signinlxqdlist> res;
		for (auto& i : qiandao) {
			res.push_back({ i.first,i.second.lxqd });
		}
		return res;
	}
}

void load() {
	db->iter([](string_view key, string_view val)->bool {
		if (key._Starts_with("qiandao_")) {
			QianDao np;
			RBStream rs(val);
			rs.apply(np);
			auto name = np.name;
			qiandao.emplace(name, np);
		}
		return true;
		});
}

void entry() {
	db = MakeKVDB(GetDataPath("FineSignIn"));
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("qdd", "", 0);
		CmdOverload(qdd, oncmd_signin);
		});
	load();
}

