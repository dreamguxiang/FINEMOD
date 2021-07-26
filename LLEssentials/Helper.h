#pragma once
#include "pch.h"
#include <mc/mass.h>
#include "Logger1.h"
#pragma warning(disable:4996)

using namespace std;
extern unordered_map<string, string> CMDMAP, CMDSCHEDULE;
extern int FAKE_SEED, MAX_CHAT_LEN, MAXONLINEPLAYER, FAKE_ONLINEPLAYER, TPRNUM;
extern unordered_set<short> logItems, banItems;
extern bool LOG_CMD, LOG_CHAT, regABILITY, nocheatcmd,Force_ResourcePacks, NO_EXPLOSION, EXP_PLAY, penderman, pfarm, fakeonlineplayer, ENABLETPR,ENABLERUNAS,ENABLETPS,DynamicPlayer, ENABLEBAN, ENABLESKICK, ENABLEVANISH, ENABLECNAME, ENABLETRANSFER, ENABLEGMODE, ENABLELORE;
extern Minecraft* mc;
extern Logger1 LOG1;
extern playerMap<string> ORIG_NAME;
extern unordered_map<string, string> CNAME;
extern unordered_map<string, int> jzqz;
extern const string version;

extern bool checkLandOwnerRange_stub(IVec2, IVec2, int, unsigned long long);
void sschTask();
void loadCfg();

struct Vec2 {
	int x, z;
	Vec2(Vec3 l) {
		x = iround(l.x);
		z = iround(l.z);
	}
	Vec2(int a, int b) :x(a), z(b) {}
	void operator+=(int v) {
		x += v;
		z += v;
	}
};