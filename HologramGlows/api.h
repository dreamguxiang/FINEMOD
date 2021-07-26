#pragma once

extern string getmoney(WPlayer& wp);
extern string gettime();
extern string getversion();
extern string getProtocolVersion();
extern string getplayerpermlevel(WPlayer& wp);
extern string GetdiskdriveByCmd(string&, int);
extern string GetCpuByCmd(string& ider, int);
extern string GetosByCmd(string& ider, int);
extern string getallmon();
extern FARPROC FNS;
//extern string ping(WPlayer& wp);

extern string diskdrivename;
extern string cpuname;
extern string osname;
