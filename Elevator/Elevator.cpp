#include <iostream>
#include <lbpch.h>
#include "pch.h"
#include <loader/Loader.h>
#include <api/MC.h>
#include <mc/Player.h>
#include <rapidjson/document.h>
#include<streambuf>
#include<api\regCommandHelper.h>
#include<api/basicEvent.h>
#include<api/types/types.h>
#include "Helper.h"
#include <map>
using namespace std;
long a;


std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	//std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);
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
	if (filen == "plugins\\iconElevator\\Elevator.json")
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
	ifstream in("plugins\\iconElevator\\Elevator.json");
	ostringstream tmp;
	tmp << in.rdbuf();
	in.close();
	string data = tmp.str();
	config.Parse(data.c_str());
	int size = config.Size();
	if (size == 0)
		cout << "[" << gettime() << u8" Error] No Member Found!!!" << endl;
}
/*
map<string, string > ele;
std::map < string, string > ::reverse_iterator iter2;
short ticks = 0;
THook(void, "?normalTick@Player@@UEAAXXZ", Player* player) {
	ticks++;
	if (ticks == 25) {
		WPlayer wp = WPlayer(*player);
		auto item = player->getSelectedItem();
		string itemname2 = item.getCustomName();
		string playername = wp->getNameTag();
		auto mstr = item.getId();
		string mstr2 = "";
		string Operatingitems = config["Operatingitems"].GetString();
		if (!item.getId() == 0) {
			SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
				void, const Item*, string*)(item.getItem(), &mstr2);
		}
		if (Operatingitems == mstr2) {
			if (itemname2 == "elevator" || itemname2 == "电梯") {
				for (iter2 = ele.rbegin(); iter2 != ele.rend(); iter2++)
					if (iter2->first == playername) {
						if (iter2->second == "null") {
							wp.sendText(u8"当前电梯模式-无", JUKEBOX_POPUP);
						}
						if (iter2->second == "up") {
							wp.sendText(u8"当前电梯模式-上升模式", JUKEBOX_POPUP);
						}
						if (iter2->second == "down") {
							wp.sendText(u8"当前电梯模式-下降模式", JUKEBOX_POPUP);
						}
					}
			}
		}else if (mstr == 0) {
			for (iter2 = ele.rbegin(); iter2 != ele.rend(); iter2++)
				if (iter2->first == playername) {
					if (iter2->second == "up") {
						wp.sendText(u8"当前电梯模式-上升模式", JUKEBOX_POPUP);
					}
					if (iter2->second == "down") {
						wp.sendText(u8"当前电梯模式-下降模式", JUKEBOX_POPUP);
					}
				}
		}
		ticks = 0;
	}
	return original(player);
}

using namespace std;
THook(bool, "?useItem@GameMode@@UEAA_NAEAVItemStack@@@Z",
	void* _this, ItemStack* item) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(_this) + 8);
	auto pl = WPlayer(*(ServerPlayer*)sp);
	string playername = sp->getNameTag();
	string mstr2 = "";
	if (!item->getId() == 0) {
		SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			void, const Item*, string*)(item->getItem(), &mstr2);
	}
	string itemname1 = item->getCustomName();
	string Operatingitems = config["Operatingitems"].GetString();
		if (Operatingitems == mstr2) {
			if (itemname1 == "elevator" || itemname1 == "电梯") {
				for (iter2 = ele.rbegin(); iter2 != ele.rend(); iter2++)
					if (iter2->first == playername) {
						if (iter2->second == "null") {
							ele[playername] = "up";
							pl.sendText(u8"您已切换至上升模式", JUKEBOX_POPUP);
							return original(_this, item);
						}
						else if (iter2->second == "up") {
							ele[playername] = "down";
							pl.sendText(u8"您已切换至下降模式", JUKEBOX_POPUP);
							return original(_this, item);
						}
						else if (iter2->second == "down") {
							ele[playername] = "null";
							pl.sendText(u8"您已退出电梯模式", JUKEBOX_POPUP);
							return original(_this, item);
						}
					}
			}
	}
	return original(_this, item);
}
*/
map<string , long > mc;
std::map < string, long > ::reverse_iterator iter;
/*
THook(bool,
	"?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* _this, ItemStack* item, BlockPos* blkpos, unsigned __int8 a4, void* v5, Block* a6) {
	long a = getTimeStamp();
	Player* sp = offGameMode::getPlayer(_this);
	auto playername = sp->getNameTag();
	for (iter = mc.rbegin(); iter != mc.rend(); iter++)
		if (iter->first == playername) {
			if (a - mc[playername] <= (long)100)
				return false;
		}
	mc[playername] = a;
	mc.insert(std::map < string, long > ::value_type(playername, a));
	Vec3 playerPos = sp->getPos();
	auto pl = WPlayer(*(ServerPlayer*)sp);
	bool mmm = false;
	auto px = (int)playerPos.x;
	auto py = (int)playerPos.y;
	auto pz = (int)playerPos.z;
	if (px < 0)px = px - 1;
	if (pz < 0)pz = pz - 1;
	int bx = blkpos->x;
	int by = blkpos->y;
	int bz = blkpos->z;
	//std::cout<< playerPos.x <<" " << playerPos.y << " " << playerPos.z << " " << px <<" "<< py<<" " << " " << pz << " " << bx << " " << by<< " " << bz<<endl;
	BlockSource* bs = *(BlockSource**)(*((__int64*)_this + 1) + 840i64);
	Block* pBlk = SymBlockSource::getBlock(bs, blkpos);
	string bname = offBlock::getFullName(pBlk);
	string pname = config["eBlockName"].GetString();
	int maxUpDis = config["maxUpDis"].GetInt();
	if (pname == bname && bx == px && bz == pz) {
		for (int i = 1; i <= maxUpDis; i++) {
			Block* tblock = SymBlockSource::getBlockint(bs, bx, by - i, bz);
			string testbname = offBlock::getFullName(tblock);
			if (testbname == bname) {
				liteloader::runcmdEx("playsound tile.piston.in @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by) + " " + std::to_string(bz) + " 1 1");
				auto q = py - i - 1;
				Vec3 pcmp2 = { playerPos.x ,q , playerPos.z };
				pl.teleport(pcmp2, pl->getDimensionId());
				//liteloader::runcmdEx("effect @a[name = \"" + playername + "\"] " + " resistance 1 255 true");
				liteloader::runcmdEx("playsound tile.piston.out @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by + i + 1) + " " + std::to_string(bz) + " 1 1");
				mmm = true;
				return false;
			}
		}
		if (!mmm) {
			pl.sendText(u8"未在下方找到电梯方块");
			return false;
		}
	}
	return original(_this, item, blkpos, a4, v5, a6);
}

THook(float, "?getDestroySpeed@Player@@QEBAMAEBVBlock@@@Z",
	Player* sp, Block* pBlk) {
	long a = getTimeStamp();
	auto playername = sp->getNameTag();
	for (iter = mc.rbegin(); iter != mc.rend(); iter++)
		if (iter->first == playername) {
			if (a - mc[playername] <= (long)100)
				return original(sp, pBlk);
		}
	mc[playername] = a;
	mc.insert(std::map < string, long > ::value_type(playername, a));
	Vec3 playerPos = sp->getPos();
	auto pl = WPlayer(*(ServerPlayer*)sp);
	bool mmm = false;
	auto px = (int)playerPos.x;
	auto py = (int)playerPos.y;
	auto pz = (int)playerPos.z;
	if (px < 0)px = px - 1;
	if (pz < 0)pz = pz - 1;
	int bx = px;
	int by = py - 2;
	int bz = pz;
	BlockSource* bs = offActor::getBlockSource(sp);
	string bname = offBlock::getFullName(pBlk);
	string pname = config["eBlockName"].GetString();
	int maxUpDis = config["maxUpDis"].GetInt();
	if (pname == bname && bx == px && bz == pz)
	{
		for (int i = 1; i <= maxUpDis; i++) {
			Block* tblock = SymBlockSource::getBlockint(bs, bx, by + i, bz);
			string testbname = offBlock::getFullName(tblock);
			if (testbname == bname) {
				liteloader::runcmdEx("playsound tile.piston.in @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by) + " " + std::to_string(bz) + " 1 1");
				auto q = py + i - 1;
				Vec3 pcmp2 = { playerPos.x ,q , playerPos.z };
				pl.teleport(pcmp2, pl->getDimensionId());
				liteloader::runcmdEx("playsound tile.piston.out @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by + i + 1) + " " + std::to_string(bz) + " 1 1");
				mmm = true;
				return original(sp, pBlk);
			}
		}
		if (!mmm) {
			pl.sendText(u8"未在上方找到电梯方块");
			return original(sp, pBlk);
		}
	}
}
*/
THook(void, "?jumpFromGround@Player@@UEAAXXZ", Player* sp) {
	Vec3 playerPos = sp->getPos();
	auto playername = offPlayer::getRealName(sp);
	auto pl = WPlayer(*(ServerPlayer*)sp);
	bool mmm = false;
	auto px = (int)playerPos.x;
	auto py = (int)playerPos.y;
	auto pz = (int)playerPos.z;
	if (px < 0)px = px - 1;
	if (pz < 0)pz = pz - 1;
	int bx = px;
	int by = py - 2;
	int bz = pz;
	BlockSource* bs = offActor::getBlockSource(sp);
	auto pBlk = SymBlockSource::getBlockint(bs, bx, by, bz);
	string bname = offBlock::getFullName(pBlk);
	string pname = config["eBlockName"].GetString();
	int maxUpDis = config["maxUpDis"].GetInt();
	if (pname == bname && bx == px && bz == pz)
	{
		for (int i = 1; i <= maxUpDis; i++) {
			Block* tblock = &bs->getBlock( bx, by + i, bz);
			string testbname = offBlock::getFullName(tblock);
			if (testbname == bname) {
				liteloader::runcmdEx("playsound tile.piston.in @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by) + " " + std::to_string(bz) + " 1 1");
				auto q = py + i - 1;
				Vec3 pcmp2 = { playerPos.x ,q , playerPos.z };
				pl.teleport(pcmp2, pl->getDimensionId());
				liteloader::runcmdEx("playsound tile.piston.out @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by + i + 1) + " " + std::to_string(bz) + " 1 1");
				mmm = true;
				return original(sp);
			}
		}
		if (!mmm) {
			pl.sendText(u8"未在上方找到电梯方块");
			return original(sp);
		}
	}
	return original(sp);
}

THook(void, "?sendActorSneakChanged@ActorEventCoordinator@@QEAAXAEAVActor@@_N@Z"
	, void* _this, Actor* ac, bool a3) {
	if (a3 == 1) {
		auto sp = MakeSP(ac);
		if (sp) {
			Vec3 playerPos = sp->getPos();
			auto playername = offPlayer::getRealName(sp);
			auto pl = WPlayer(*(ServerPlayer*)sp);
			bool mmm = false;
			auto px = (int)playerPos.x;
			auto py = (int)playerPos.y;
			auto pz = (int)playerPos.z;
			if (px < 0)px = px - 1;
			if (pz < 0)pz = pz - 1;
			int bx = px;
			int by = py - 2;
			int bz = pz;
			//std::cout<< playerPos.x <<" " << playerPos.y << " " << playerPos.z << " " << px <<" "<< py<<" " << " " << pz << " " << bx << " " << by<< " " << bz<<endl;
			BlockSource* bs = offActor::getBlockSource(sp);
			auto pBlk = SymBlockSource::getBlockint(bs, bx, by, bz);
			string bname = offBlock::getFullName(pBlk);
			string pname = config["eBlockName"].GetString();
			int maxUpDis = config["maxUpDis"].GetInt();
			if (pname == bname && bx == px && bz == pz) {
				for (int i = 1; i <= maxUpDis; i++) {
					Block* tblock = &bs->getBlock(bx, by - i, bz);
					string testbname = offBlock::getFullName(tblock);
					if (testbname == bname) {
						liteloader::runcmdEx("playsound tile.piston.in @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by) + " " + std::to_string(bz) + " 1 1");
						auto q = py - i - 1;
						Vec3 pcmp2 = { playerPos.x ,q , playerPos.z };
						pl.teleport(pcmp2, pl->getDimensionId());
						//liteloader::runcmdEx("effect @a[name = \"" + playername + "\"] " + " resistance 1 255 true");
						liteloader::runcmdEx("playsound tile.piston.out @a[name=\"" + playername + "\"] " + std::to_string(bx) + " " + std::to_string(by + i + 1) + " " + std::to_string(bz) + " 1 1");
						mmm = true;
						return original(_this, ac, a3);
					}
				}
				if (!mmm) {
					pl.sendText(u8"未在下方找到电梯方块");
					return original(_this, ac, a3);
				}
			}
		}
	}
	return original(_this, ac, a3);
}


void entry() {
	loadconfig();
	//Join();
	//mode = true;
	cout << "[Elevator] Elevator Loaded, By DreamGuXiang, Build Date [" << __TIMESTAMP__ << u8"] @FineServer丨iFine " << endl;
	cout << u8"[方块电梯] 方块电梯已加载, 作者 DreamGuXiang, 构建时间 [" << __TIMESTAMP__ << u8"] @FineServer丨iFine " << endl;
}
