#include <iostream>
#include <string.h>
#include <lbpch.h>
#include <loader/Loader.h>
#include <string>
#include <JsonLoader.h>
#include <chrono>
#include <fstream>
#include <mc/Player.h>
#include <stl\varint.h>
#include <api/xuidreg/xuidreg.h>
#include <api/LiteLoaderApi.h>
#include <mc/Item.h>
#include <random>
#include <windows.h>
#include "log.h"
#include <api/scheduler/scheduler.h>
#include<api\regCommandHelper.h>
#include <api/myPacket.h>
#include <map>
#include <set>
#include"hash_Set.h"
#include "hash_set.cpp"
std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	return timestamp;
}
int _access(const char
	* path, int mode);

using namespace std;
#pragma warning(disable:4996)

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
	if (filen == "plugins\\ClearLag\\ClearLag.json")
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
	ifstream in("plugins\\ClearLag\\ClearLag.json");
	ostringstream tmp;
	tmp << in.rdbuf();
	in.close();
	string data = tmp.str();
	config.Parse(data.c_str());
	int size = config.Size();
	if (size == 0)
		cout << "[" << gettime() << u8" Error] No Member Found!!!" << endl;
}

unordered_map<string, set<Actor*>> actors;
SRWLOCK lock;
THook(void, "?updateTickingData@Actor@@QEAAXXZ",
	Actor* a1){
	if(MakeSP(a1))
		return original(a1);
	auto name = SymCall("?getActorName@CommandUtils@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z", string, Actor*)(a1);
	//cout << name <<endl;
	AcquireSRWLockExclusive(&lock);
	actors[name].insert(a1);
	ReleaseSRWLockExclusive(&lock);
	return original(a1);
}
int nums = 0;
namespace bc {
	void bctext(string a1, TextType a2) {
		auto pls = liteloader::getAllPlayers();
		for (auto i : pls) {
			WPlayer(*i).sendText(a1, a2);
		}
	}
}

hash_set<void*> acc;
void acccc() {
	for (auto& v : config["ClearList"].GetArray()) {
			string bstr = "??_7" + (string)v.GetString() + "@@6B@";
			auto typeee = dlsym_real(bstr.c_str());
			acc.insert(typeee);
	}
	auto type1= dlsym_real("??_7Actor@@6B@");
	acc.insert(type1);
	auto type2 = dlsym_real("??_7Mob@@6B@");
	acc.insert(type2);
	auto type3 = dlsym_real("??_7Animal@@6B@");
	acc.insert(type3);
	auto type4 = dlsym_real("??_7Monster@@6B@");
	acc.insert(type4);
	auto type5 = dlsym_real("??_7ItemActor@@6B@");
	acc.insert(type5);
}
inline static Actor* MakeAC(void* x) {
	if (!x)
		return nullptr;
	//cout << dAccess<void*, 0>(x) << endl;
	if (dAccess<void*, 0>(x) != x) {
		return (Actor*)x;
	}
	return nullptr;
}
void remove(Actor* a1) {
	auto type1 = dlsym_real("??_7Pig@@6B@");
	auto acbool = SymCall("?isRemoved@Actor@@QEBA_NXZ", __int64, Actor*)(a1);
	cout <<u8"原始指针："<< a1 << u8" 偏移后指针：" << dAccess<void*, 0>(a1) << u8" PIG统一指针："<<type1 <<u8" isremove："<< acbool <<endl;
	if (dAccess<void*, 0>(a1) != a1) { 
		SymCall("?remove@Actor@@UEAAXXZ", void, Actor*)(a1);
		cout << u8"执行成功" << endl;
	}
	++nums;
}

#include <api/scheduler/scheduler.h>
#include <malloc.h>
map<string, int > tick;
void schTask() {
	std::thread t([] {
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在60秒后清理", RAW);
		//std::this_thread::sleep_for(std::chrono::seconds(30));
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在30秒后清理", RAW);
		//std::this_thread::sleep_for(std::chrono::seconds(20));
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在10秒后清理", RAW);
		//std::this_thread::sleep_for(std::chrono::seconds(7));
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在3秒后清理", RAW);
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在2秒后清理", RAW);
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		bc::bctext(u8"§6[§eClearLag§6]§r 服务器生物与掉落物将在1秒后清理", RAW);
		long start = getTimeStamp();
		for (auto& v : config["ClearList"].GetArray()) {
			long starts = getTimeStamp();
			int num = 0;
			AcquireSRWLockShared(&lock);
			for (auto actor : actors[v.GetString()])
			{
				//cout << actor << endl;
				remove(actor);
				num++;
			}
			ReleaseSRWLockShared(&lock);
			long ends = getTimeStamp();
			tick.insert(std::map < string, int > ::value_type(v.GetString(), num));
		}
		long end = getTimeStamp();
		bc::bctext(u8"§6[§eClearLag§6]§r 清理成功！共计清除 " + to_string(nums) + u8"个生物 总耗时：" + to_string(end - start) + u8"ms\n详细如下", RAW);
		cout << u8"§6[§eClearLag§6]§r 清理成功！共计清除 " << to_string(nums) << u8"个生物 总耗时：" << to_string(end - start) << u8"ms\n详细如下" << endl;
		map<string, int>::reverse_iterator iter; 
		for (iter = tick.rbegin(); iter != tick.rend(); iter++) {

			bc::bctext(u8"类型：" + iter->first +u8" 共计："+ to_string(iter->second), RAW);
			cout << u8"类型：" << iter->first << u8" 共计："  << to_string(iter->second) << endl;
		}
		map<string, int >().swap(tick);
		map<string, int >().clear();
		unordered_map<string, set<Actor*>>().swap(actors);
		unordered_map<string, set<Actor*>>().clear();
		nums = 0;
		});
	t.detach();
}
void timer() {
	Handler::schedule(RepeatingTask([] {
		schTask();
		}, config["timer"].GetInt()*2));
}
void entry() {
	loadconfig();
	Event::addEventListener([](ServerStartedEV ev) {
		InitializeSRWLock(&lock);
		//acccc();
		timer();
	});
}
