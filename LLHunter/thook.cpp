#include <lbpch.h>
#include <iostream>
#include "pch.h"
#include <llmoney.h>
#include <loader/Loader.h>
#include <api/MC.h>
#include <filesystem>
#include <mc/Player.h>
#include<api\regCommandHelper.h>
#include <mc/mass.h>
#include "random"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include<mc/OffsetHelper.h>

using namespace std;
std::unique_ptr<KVDBImpl> db;

int _access(const char
	* path, int mode);

using namespace std;
#pragma warning(disable:4996)

struct KillMobNum {
	long num;
	string name;
#define SBIT(x) (1ull<<x)
	unsigned long long flag = SBIT(14) | SBIT(15);
	void pack(WBStream& bs)const {
		bs.apply(num,name);
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
		bs.apply(num,name);
		string val;
	}
};

inline string gettime()
{
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);
	strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", LocTime);
	return string(timestr);
}

inline void fw(const string& filen, const string& instr)
{
	if (filen == "plugins\\LLhunter\\LLHunter.json")
		return;
	if ((_access(filen.c_str(), 0)) != -1)
	{
		ofstream outfile;
		outfile.open(filen, ios::app);
		if (!outfile)
		{
			cout << "[" << gettime() << u8" INFO] FileWriteFailed!!!" << endl;
		}
		outfile << instr << endl;
		outfile.close();
	}
	else
	{
		std::ofstream outfile(filen);
		if (!outfile)
		{
			cout << "[" << gettime() << u8" INFO] FileWriteFailed!!!" << endl;
		}
		unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
		outfile.write((char*)bom, sizeof(bom));
		outfile.close();
		fw(filen, instr);
	}
}

rapidjson::Document config;
void loadconfig() {
	ifstream in("plugins\\LLhunter\\LLHunter.json");
	ostringstream tmp;
	tmp << in.rdbuf();
	in.close();
	string data = tmp.str();
	config.Parse(data.c_str());
	int size = config.Size();
	if (size == 0)
		cout << "[" << gettime() << u8" Error] No Member Found!!!" << endl;
}

#include <map>
#include <api/scheduler/scheduler.h>

map<string, long > killmob;
std::map < string, long > ::reverse_iterator iter;

THook(void, "?die@Mob@@UEAAXAEBVActorDamageSource@@@Z", Mob* _this, ActorDamageSource* a2) {
	string Mob_name = _this->getNameTag();
	string Mob_name2 = SymCall("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$all"
		"ocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z", string, int, int)(_this->getEntityTypeId(), 1);
	int srccode = dAccess<int>(a2, 8);
	if (srccode == 2 || srccode == 3) {
		Player* spl = _this->getLastHurtByPlayer();
		auto pl = WPlayer(*(ServerPlayer*)spl);
		if (pl) {
			auto plname = offPlayer::getRealName(pl);
			for (iter = killmob.rbegin(); iter != killmob.rend(); iter++) {
				if (iter->first == plname) {
					killmob[plname] = iter->second + 1;
				}
			}
			for (auto& m : config.GetObj())
				if (m.name.GetString() == Mob_name2) {
					long money = Money::getMoney(offPlayer::getXUID(spl));
					int max = config[m.name.GetString()]["max"].GetInt();
					int min = config[m.name.GetString()]["min"].GetInt();
					auto num1 = rand() % (max - min + 1) + min;
					Money::createTrans(0, offPlayer::getXUID(spl), num1);
					string zhname = config[m.name.GetString()]["name"].GetString();
					if (config[m.name.GetString()]["serverallplayers"].GetBool() == false)
					{
						for (iter = killmob.rbegin(); iter != killmob.rend(); iter++)
							if (iter->first == plname) {
								pl.sendText(u8"§6成功击杀§d " + zhname + u8" §6获得了§e" + std::to_string(num1) + config["coinname"].GetString()
									+ u8" §6当前余额:§b" + std::to_string(money + num1) + config["coinname"].GetString() + u8" §6累计击杀:§g" + to_string(iter->second), JUKEBOX_POPUP);
							}
					}
					else
					{
						pl.sendText(u8"§6成功击杀§d " + zhname + u8" §6获得了§e" + std::to_string(num1) + config["coinname"].GetString()
							+ u8" §6当前余额:§b" + std::to_string(money + num1) + config["coinname"].GetString(), JUKEBOX_POPUP);
						liteloader::runcmdEx(u8"tellraw @a {\"rawtext\":[{\"text\":\"§6玩家§b" + offPlayer::getRealName(spl)
							+ u8"§6成功击杀§d " + zhname + u8" §6获得了§e" + std::to_string(num1) + config["coinname"].GetString() + "\"}]}");
					}
				}
		}
	}
	return original(_this, a2);
}

#include <llhunter.h>
namespace Hunter {
	LLHUNTER_API std::vector<killmoblist> getkillmoblist() {
		std::vector<killmoblist> res;
		for (auto& i : killmob) {
			res.push_back({ i.first,i.second });
		}
		return res;
	}
}

THook2("玩家保存", void, "?saveLevelData@Level@@UEAAXXZ", Level* a1) {
	for (iter = killmob.rbegin(); iter != killmob.rend(); iter++) {
		KillMobNum killm;
		killm.num = iter->second;
		killm.name = iter->first;
		WBStream ws;
		ws.apply(killm);
		db->put("killmob_" + iter->first, ws);
	}
	return original(a1);
}
void load() {
	db->iter([](string_view key, string_view val)->bool {
		if (key._Starts_with("killmob_")) {
			KillMobNum np;
			RBStream rs(val);
			rs.apply(np);
			auto num = np.num;
			auto name = np.name;
			killmob.emplace(name, num);
		}
		return true;
		});
}



#pragma region CMDENUM
enum class huntercmd :int {
	reload = 1
};
#pragma endregion 


#pragma region cmd
bool reconfig(CommandOrigin const& ori, CommandOutput& outp, MyEnum<huntercmd> op) {
	switch (op)
	{
	case huntercmd::reload: {
		std::cout << "[LLHunter] reload success" << endl;
		outp.addMessage("[LLHunter] reload success");
		loadconfig();
		break;
	}
	}
}

bool oncmd_hunter(CommandOrigin const& ori, CommandOutput& outp, MyEnum<huntercmd> op) {
	return reconfig(ori, outp, op);
}
#pragma endregion

void entry() {
	loadconfig();
	db = MakeKVDB(GetDataPath("killMob"));
	load();
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		CEnum<huntercmd> _1("Huntercmd", { "reload" });
		MakeCommand("hunter", "hunter menu", 1);
		CmdOverload(hunter, oncmd_hunter, "op");
		});
	cout << "[LLHunter] LLHunter Loaded, By DreamGuXiang, Build Date [" << __TIMESTAMP__ << u8"]" << endl;
}

THook(void*, "?onPlayerJoined@ServerScoreboard@@UEAAXAEBVPlayer@@@Z",
	void* _this, Player* a2) {
	if (killmob.find(offPlayer::getRealName(a2)) == killmob.end()) {
		killmob[offPlayer::getRealName(a2)] = 0;
	}
	return original(_this, a2);
}