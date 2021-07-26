// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <iostream>
using namespace std;
double MoneyFee;

#include "pch.h"
#include <loader/Loader.h>

////#include <unordered_map>
//#include<api/types/helper.h>
//#include <stl/optional.h>
//#include <liteloader.h>
//#include <mc/Core.h>
//#include <mc/mass.h>
#include <api/types/types.h>
#include <mc/Block.h>
using namespace std;

THook(void, "?disconnectClient@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@EAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
	void* a1, void* a2, unsigned char a3, std::string a4, bool a5) {
	//std::cout << msg << endl;
	if (false) {}
	//else if (msg == "disconnectionScreen.disconnected") { msg = u8"断开连接"; }
	else
		if (a4 == "disconnectionScreen.notAuthenticated") { 
			a4 = u8"§6未能通过Microsoft服务的身份验证\n§a       请检查你的网络环境";
		}
	else 
			if (a4 == "disconnectionScreen.notAllowed") { 
				a4 = u8"§6         你不在FINE的白名单内！\n§e(MCBE FINE开启了白名单,请向管理员申请白名单)"; 
			}
	//std::cout << msg << endl;
	original(a1, a2, a3, a4, a5);
	//else if (msg == "disconnectionScreen.invalidTenant") { msg = u8"断开连接"; }
	//else if (msg == "disconnectionScreen.outdatedServer") {
	//	/*int ServerNetworkProtocolVersion = f(int, dlsym("?NetworkProtocolVersion@SharedConstants@@3HB"));
	//		msg = ;*/
	//}
	//else if (msg == "disconnectionScreen.outdatedClient") {
	//	msg = u8"你的客户端版本太低，无法加入服务器";
	//}
	//else if (msg == "disconnectionScreen.serverIdConflict") { msg = u8"断开连接"; }
	//else if (msg == "disconnectionScreen.multiplayerDisabled") { msg = u8"断开连接"; }
	//else if (msg == "disconnectionScreen.editionMismatchEduToVanilla") { msg = u8"断开连接"; }
	//else if (msg == "disconnectionScreen.editionMismatchVanillaToEdu") { msg = u8"断开连接"; }
	//else if (msg == "disconnectionScreen.invalidSkin") { msg = u8"断开连接（无效的皮肤）"; }
	//"disconnectionScreen.disconnected"
	//"disconnectionScreen.notAuthenticated"
	//"disconnectionScreen.serverIdConflict"
	//"disconnectionScreen.multiplayerDisabled"
	//"disconnectionScreen.editionMismatchEduToVanilla"
	//"disconnectionScreen.editionMismatchVanillaToEdu"
	//"disconnectionScreen.invalidTenant"
	//"disconnectionScreen.notAllowed"
	//"disconnectionScreen.invalidSkin"
	//"disconnectionScreen.disconnected"
	//"disconnectionScreen.outdatedServer"
	//"disconnectionScreen.outdatedClient"
}


THook(void, "?write@PlayStatusPacket@@UEBAXAEAVBinaryStream@@@Z",
	void* a0, void* a1) {
	int state = *(int*)(((uintptr_t)a0) + 48);
	if (state == 1 || state == 2) { return; }
	if (state > 3 && state < 6) { return; }

	if (state == 0) *(int*)(((uintptr_t)a0) + 48) = 0;
	//std::cout << state << endl;

	original(a0, a1);
	//Play Status constants
	//Name	Value
	//Login Success	0
	//Login Failed Client	1
	//Login Failed Server	2
	//Player Spawn	3
	//Login Failed Invalid Tenant	4
	//Login Failed Vanilla Edu	5
	//Login Failed Edu Vanilla	6
	//Login Failed Server Full	7
}

//THook(void, "?onDisconnect@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N1@Z",
//	void* a1, void* a2, void* a3, void* a4, void* a5 ) {
//	f(string, a5) = u8"爪巴！";
//	f(string, a3) = u8"爪巴！";
//	std::cout << "[" << gettime() << u8" INFO][TEST] [EXonDisconnect3]" << *(string*)(a3) << endl;
//	std::cout << "[" << gettime() << u8" INFO][TEST] [EXonDisconnect5]" << *(string*)(a5) << endl;
//	 //original(a1, a2, a3, a4, &testmsg);
//}

THook(void*, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVLoginPacket@@@Z", void* a, void* b, void* c) {
	int pv = *(int*)(((uintptr_t)c) + 48);
	auto ServerNetworkProtocolVersion = SymCall("?NetworkProtocolVersion@SharedConstants@@3HB",int*);
	if (pv != *(int*)ServerNetworkProtocolVersion) {
		auto MajorVersion = SymCall("?MajorVersion@SharedConstants@@3HB", int*);
		auto MinorVersion = SymCall("?MinorVersion@SharedConstants@@3HB", int*);
		auto PatchVersion = SymCall("?PatchVersion@SharedConstants@@3HB", int*);
		auto RevisionVersion = SymCall("?RevisionVersion@SharedConstants@@3HB", int*);
		string tip =
			u8"\n  §r§b服务端支持游戏版本：§l§6" + to_string(*(int*)MajorVersion) + "." + to_string(*(int*)MinorVersion) +
			"." + to_string(*(int*)PatchVersion) + u8"§g." + to_string(*(int*)RevisionVersion) +
			u8"\n      §r§e服务端协议版本： §l§a" + to_string(*(int*)ServerNetworkProtocolVersion) +
			u8"\n      §r§e客户端协议版本： §c§l" + to_string(pv);
		if (pv < *(int*)ServerNetworkProtocolVersion)
		{
			string msg = u8"§c你的客户端版本太低，无法加入服务器" + tip;
			SymCall("?disconnectClient@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@EAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
				__int64, void*, void*, unsigned __int8, string, char)(a, b, 0, msg, 0);
		}
		else if (pv > *(int*)ServerNetworkProtocolVersion) {
			string msg = u8"§c你的客户端版本过新，无法加入服务器" + tip;
			SymCall("?disconnectClient@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@EAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
				__int64, void*, void*, unsigned __int8, string, char)(a, b, 0, msg, 0);
		}
	}
	return original(a, b, c);
}

void entry() {

}

extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
		entry();
    }
}


