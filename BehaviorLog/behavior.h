#include <loader/Loader.h>
#include <string>
#include <iostream>
#include <JsonLoader.h>
#include <time.h>
#include <sstream>
#include <chrono>
#include <stdio.h>
#include <fstream>
#include <future>
#include <stl/langpack.h>
#include "csvwriter.h"
#include  <io.h>

#pragma warning(disable:4996)
using namespace std;
inline bool mode = false;
typedef signed int          INT32, * PINT32;

inline bool logCmdMlock = true;
inline bool CmdBlockWriteable = true;
inline bool force_achievement = true; 

inline bool LogUseItem = true;
inline bool LogChest = true;
inline bool LogPlace = true;
inline bool LogDestory = true;
inline bool LogCDim = true;
inline bool LogDie = true;
inline bool LogFire = true;

inline bool showLogUseItem = true;
inline bool showLogChest = true;
inline bool showLogBlock = true;
inline bool showLogCDim = true;
inline bool showLogDie = true;
inline bool showLogFire = true;
inline bool showLogTakeDrop = true;

inline bool LogCMD = true;
inline bool LogJoin = true;
inline bool LogLeft = true;
inline bool LogChat = true;
inline bool LogTakeDrop = true;
inline LangPack LangP("plugins/LLBehaviorLog/langpack/behaviorloglang.json");

inline string loginlogfile = "logs/LoginLog.csv";
inline string blocklogfile = "logs/BlockLog.csv";
inline string cmdlogfile= "logs/CMDLog.csv";
inline string containerlogfile="logs/ContainerLog.csv";
inline string attacklogfile= "logs/AttackLog.csv";
inline string commonlogfile= "logs/CommonLog.csv";
inline void loadconf() {
	ifstream cfile;
	cfile.open("plugins/LLBehaviorLog/behaviorlog.json");
	if (cfile)
	{
		cfile.close();
		try {
			ConfigJReader jr("plugins/LLBehaviorLog/behaviorlog.json");
			jr.bind("LogUseItem", LogUseItem, true);
			jr.bind("LogChest", LogChest, true);
			jr.bind("LogPlace", LogPlace, true);
			jr.bind("LogDestory", LogDestory, true);
			jr.bind("LogCDim", LogCDim, true);
			jr.bind("LogJoin", LogJoin, true);
			jr.bind("LogLeft", LogLeft, true);
			jr.bind("LogDie", LogDie, true);
			jr.bind("LogTakeDrop", LogTakeDrop, true);
			jr.bind("showLogUseItem", showLogUseItem, false);
			jr.bind("showLogChest", showLogChest, true);
			jr.bind("showLogBlock", showLogBlock, false);
			jr.bind("showLogCDim", showLogCDim, true);
			jr.bind("showLogDie", showLogDie, true);
			jr.bind("showLogFire", showLogFire, true);
			jr.bind("showLogTakeDrop", showLogTakeDrop, true);

			jr.bind("logCmdMlock", logCmdMlock, true);
			jr.bind("CmdBlockWriteable", CmdBlockWriteable, true);
			jr.bind("ForceAchievement", force_achievement, true);
		}
		catch (string e) {
			printf("[BehaviorLog]JSON ERROR %s\n", e.c_str());
			exit(1);
		}
	}
	else {
		cfile.close();
		cout << _TRS("no.config.usedefault") << endl;
	}
}
template <size_t size> void UtoA_Fill(char(&buf)[size], int num) {
	int nt = size - 1;
	buf[nt] = 0;
	for (auto i = nt - 1; i >= 0; --i) {
		char d = '0' + (num % 10);
		num /= 10;
		buf[i] = d;
	}
}

inline string getday() {
	auto timet = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm time;
	char buf[3] = { 0 };
	localtime_s(&time, &timet);
	string str(to_string((time.tm_year + 1900)));
	UtoA_Fill(buf, time.tm_mon + 1);
	str += buf;
	UtoA_Fill(buf, time.tm_mday);
	str += buf;
	return str;
}
inline string gethour() {
	auto timet = chrono::system_clock::to_time_t(chrono::system_clock::now());
	tm time;
	char buf[3] = { 0 };
	localtime_s(&time, &timet);
	string str;
	UtoA_Fill(buf, time.tm_hour);
	str += buf;
	return str;
}
//[2020-08-31 18:55:50 INFO]
inline std::string gettime() {
	time_t rawtime;
	tm* LocTime;
	char timestr[20];
	time(&rawtime);
	LocTime = localtime(&rawtime);

	strftime(timestr, 20, "%Y-%m-%d %H:%M:%S", LocTime);
	return string(timestr);
}
enum logmode {
	login = 0,
	block = 1,
	cmds = 2,
	container = 3,
	attack = 4,
	common = 5,

};
inline string getfilename(enum logmode mode) {
	time_t rawtime;
	tm* LocTime;
	char timestr[50] = {0};
	time(&rawtime);
	LocTime = localtime(&rawtime);
	if(mode == 0)
		strftime(timestr, 50, loginlogfile.c_str(), LocTime);
	if (mode == 1)
		strftime(timestr, 50, blocklogfile.c_str(), LocTime);
	if (mode == 2)
		strftime(timestr, 50, cmdlogfile.c_str(), LocTime);
	if (mode == 3)
		strftime(timestr, 50, containerlogfile.c_str(), LocTime);
	if (mode == 4)
		strftime(timestr, 50, attacklogfile.c_str(), LocTime);
	if (mode == 5)
		strftime(timestr, 50, commonlogfile.c_str(), LocTime);
	return string(timestr);

}
//file write
inline void fw(const string& filen, const string& instr) {
	if (filen == "NONE")
		return;
	if ((_access(filen.c_str(), 0)) != -1)
	{
		ofstream outfile;
		outfile.open(filen, ios::app);
		if (!outfile)
		{
			cout << "[" << gettime() << u8" INFO][BehaviorLog] " << _TRS("log.failwritefile") << endl;
		}
		outfile << instr << endl;
		outfile.close();
	}
	else {
		std::ofstream outfile(filen);
		if (!outfile)
		{
			cout << "[" << gettime() << u8" INFO][BehaviorLog] " << _TRS("log.failwritefile") << endl;
		}
		unsigned char bom[] = { 0xEF,0xBB,0xBF };
		outfile.write((char*)bom, sizeof(bom));
		outfile.close();
		fw(filen, instr);
	}
}

inline string Title(const string& titll) {
	return "[" + gettime() + " " + titll + "][Log]";
}
inline string Coordinator(INT32 coordinator[]) {
	return string("(") + to_string(coordinator[0]) + ", " + to_string(coordinator[1]) + ", " +
		to_string(coordinator[2]) + ")";
}

inline string Pos(const Vec3 vec) {
	return "(" + to_string(int(vec.x)) + ", " + to_string(int(vec.y)) + ", " + to_string(int(vec.z)) + ")";
}
const string _UNK = "unknown";
inline const string& Dimension(int v) {
	switch (v) {
	case 0: return _TRS("Main");
	case 1: return _TRS("Nether");
	case 2: return _TRS("End");
	}
	return _UNK;
}
inline auto posny(INT32 coordinator[]) {
	return string(to_string(coordinator[0]) + " " + to_string(coordinator[2]));
}


namespace logger {
	inline void Error(const string& title, const string& player_name, int dimension, const string& content) {
		printf(_TR("log.Error"), Title(title).c_str(), player_name.c_str(), Dimension(dimension).c_str(), content.c_str());
	}
	inline void Block(
		const string& title, const string& player_name,  int dimension, const string& operation,
		const string& bname, INT32 pos[]) {
		auto block_name_inner = bname;
		if (block_name_inner == "") block_name_inner = _TRS("unknow.block");
		if (showLogBlock)
			printf(_TR("log.Block"), Title(title).c_str(), player_name.c_str(), Dimension(dimension).c_str(), Coordinator(pos).c_str(), operation.c_str(), block_name_inner.c_str());
		string logfile = getfilename(block);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< dimension
			<< Coordinator(pos)
			<< operation
			<< block_name_inner;
		fw(logfile, csv.getstr());
	}
	inline void Item(
		const string& title, const string& player_name, int dimension, const string& operation,
		const string& item_name, INT32 coordinator[]) {
		if (showLogUseItem)
			printf(_TR("log.Item"), Title(title).c_str(), player_name.c_str(), Dimension(dimension).c_str(), Coordinator(coordinator).c_str(), operation.c_str(), item_name.c_str());
		string logfile = getfilename(container);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< ""
			<< dimension
			<< Coordinator(coordinator)
			<< operation
			<< item_name;
		fw(logfile, csv.getstr());
	}
    inline void TakeDrop(
		const string& title, const string& player_name, int dimension, const string& operation,
		const string& item_name, Vec3 pos,int itemc) {
		if (showLogTakeDrop)
			printf(_TR("log.TakeDrop"), Title(title).c_str(), player_name.c_str(), Dimension(dimension).c_str(), Pos(pos).c_str(), operation.c_str(), item_name.c_str(),to_string(itemc).c_str());
		string logfile = getfilename(container);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< operation
			<< dimension
			<< Pos(pos)
			<< item_name
            << itemc;
		fw(logfile, csv.getstr());
	}
	inline void Interaction(
		const string& title, const string& player_name,  int dimension, const string& operation,
		const string& object_name, INT32 coordinator[]) {
		if (showLogChest)
			printf(_TR("log.Interaction"), Title(title).c_str(), player_name.c_str(), operation.c_str(), Dimension(dimension).c_str(), Coordinator(coordinator).c_str(), object_name.c_str());
		string logfile = getfilename(container);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< dimension
			<< Coordinator(coordinator)
			<< operation
			<< object_name;
		fw(logfile, csv.getstr());

	}
	inline void Container_In(
		const string& title, const string& player_name, const string& container_n, INT32 pos[],int dimension, int slot, int count,
		const string& object_name) {
		if (showLogChest)
			printf(_TR("log.ContainerIn"), Title(title).c_str(), player_name.c_str(), container_n.c_str(), Coordinator(pos).c_str(), to_string(slot).c_str(), to_string(count).c_str(), object_name.c_str());
		string logfile = getfilename(container);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< "put"
			<< container_n
			<< Coordinator(pos)
			<< dimension
			<< slot
			<< count
			<< object_name;
		fw(logfile, csv.getstr());
	}
	inline void Container_Out(const string& title, const string& player_name, const string& container_n,INT32 pos[], int dimension, int slot, const string& object_name) {
		if (showLogChest)
			printf(_TR("log.ContainerOut"), Title(title).c_str(), player_name.c_str(),container_n.c_str(),Coordinator(pos).c_str(), to_string(slot).c_str());
		string logfile = getfilename(container);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< "get"
			<< container_n
			<< Coordinator(pos)
			<< dimension
			<< slot
			<< -1
			<< object_name;
		fw(logfile, csv.getstr());
	}

	inline void ChangeDimension(const string& title, const string& player_name, int dimension, const Vec3 v) {
		if (showLogCDim)
			printf(_TR("log.Cdim"), Title(title).c_str(), player_name.c_str(), Dimension(dimension).c_str(), Pos(v).c_str());
		string logfile = getfilename(common);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< player_name
			<< ""
			<< dimension
			<< Pos(v)
			<< "changedim"
			<< "";
		fw(logfile, csv.getstr());
	}
	inline void Fire(
		const string& title, INT32 pos[]) {
		if (showLogFire)
			printf(_TR("log.fire"), Title(title).c_str(), Coordinator(pos).c_str());
		string logfile = getfilename(common);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< ""
			<< ""
			<< ""
			<< Coordinator(pos)
			<< "onFire"
			<< "";
		fw(logfile, csv.getstr());
	}
	inline void showDie(const string& title, int dim, const Vec3 pos, const string& mob_name, const string& src_name, const int src_cause) {
		if (showLogDie)
			printf(_TR("log.Die"), Title(title).c_str(), mob_name.c_str(), Dimension(dim).c_str(), Pos(pos).c_str(), src_name.c_str());
		string logfile = getfilename(attack);
		string dims = to_string(dim);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< mob_name
			<< ""
			<< dims
			<< Pos(pos)
			<< src_cause
			<< src_name;
		fw(logfile, csv.getstr());
	}
	inline void cmdbw(string title, const string& playern, const int dim, Vec3 pos, const string& cmdtext, const string& value) {
		printf(_TR("log.CmdBlockWrite"), Title(title).c_str(), playern.c_str(), Dimension(dim).c_str(), Pos(pos).c_str(), cmdtext.c_str(), value.c_str());
		string logfile = getfilename(cmds);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< playern
			<< "U"
			<< dim
			<< Pos(pos)
			<< value
			<< cmds;
		fw(logfile, csv.getstr());
	}
	inline void cmdmsg(string title, const string& playern, const int dim,const string& cmdtext) {
		//printf(_TR("log.CmdBlockWrite"), Title(title).c_str(), playern.c_str(), Dimension(dim).c_str(), cmdtext.c_str());
		string logfile = getfilename(cmds);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< playern
			<< "dim"
			<< dim
			<< "cmdtext"
			<< cmdtext;
		fw(logfile, csv.getstr());
	}
	inline void chatmsg(string title, const string& playern, const int dim, const string& chattext) {
		//printf(_TR("log.CmdBlockWrite"), Title(title).c_str(), playern.c_str(), Dimension(dim).c_str(), cmdtext.c_str());
		string logfile = getfilename(common);
		CSVWriter csv(",");
		csv.addLine()
			<< gettime()
			<< title
			<< playern
			<< "dim"
			<< dim
			<< "chattext"
			<< chattext;
		fw(logfile, csv.getstr());
	}
}
