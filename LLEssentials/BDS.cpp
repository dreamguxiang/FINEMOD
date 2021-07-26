#include "pch.h"
#include "BDS.h"
#include "Helper.h"
#include <mc/Player.h>

void foreachPlayerInv(Player* pl, function<void(ItemStackBase*)>& func) {
	auto v9 = *((uintptr_t*)pl + 380);
	auto v13 = (*(__int64(__fastcall**)(uintptr_t))(**(uintptr_t**)(v9 + 176) + 112i64))(*(uintptr_t*)(v9 + 176));
	for (int i = 0; i < v13; ++i)
	{
		auto v15 = (*(__int64(__fastcall**)(uintptr_t, uintptr_t))(**(uintptr_t**)(v9 + 176) + 40i64))(
			*(uintptr_t*)(v9 + 176),
			(unsigned int)i);
		auto v16 = (ItemStackBase*)v15;
		func(v16);
	}
}

void setPlayerGameType(Player* pl, int t) {
	SymCall("?setPlayerGameType@ServerPlayer@@UEAAXW4GameType@@@Z", void, Player*, int)(pl, t);
}

void* createPacket(int id) {
	void* ret;
	auto r = SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		void*, void**, int)(&ret, id);
	return r;
}

void sendPack(Player* pl, void* pack) {
	SymCall("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z", void, Player*, void*)(pl, pack);
}

void forceKick(Player* pl, string msg) {
	if (pl) {
		//ServerPlayer::isHostingPlayer
		//SymCall("?disconnectClient@ServerNetworkHandler@@QEAAXAEBVNetworkIdentifier@@EAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z",
		//	void*, void*, void*, unsigned __int8, string, bool)
		//	(mc->getServerNetworkHandler(), network, 0, msg, 0);
		auto nh = mc->getServerNetworkHandler();
		NetworkIdentifier* a = offPlayer::getNetworkIdentifier(pl);
		nh->disconnectClient(*(NetworkIdentifier*)a, msg, 0);
	}
}

void forEachPlayer(Level* lv, std::function<bool(Player*)> func) {
	SymCall("?forEachPlayer@Level@@UEBAXV?$function@$$A6A_NAEBVPlayer@@@Z@std@@@Z",
		void, void*, std::function<bool(Player*)>)(lv, func);
}
/*
void setServerMotd() {
#define _BYTE char
#define _QWORD uintptr_t
	auto v1 = (uintptr_t)mc->getServerNetworkHandler();
  if ( *((_BYTE *)v1 + 584) )
  {
    auto v2 = 0;
	auto v8 = *((_QWORD*)v1 + 10);
	auto v9 = *(void (__fastcall **)(__int64, char *, __int64, _QWORD, int, int, char))(*(_QWORD *)v8 + 24i64);
    auto v10 = *((int *)v1 + 180);
    auto v11 = (*(__int64 (__fastcall **)(_QWORD))(**((_QWORD **)v1 + 7) + 1136i64))(*((_QWORD *)v1 + 7));
	auto v12 = SymCall("?getGameType@LevelData@@QEBA?AW4GameType@@XZ",
		char, uintptr_t)(v11);
    auto v13 = (*(__int64 (__fastcall **)(_QWORD))(**((_QWORD **)v1 + 7) + 1136i64))(*((_QWORD *)v1 + 7));
	auto v14 = SymCall("?getLevelName@LevelData@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		__int64, uintptr_t)(v13);
    v9(v8, (char *)v1 + 600, v14, v12, FAKE_ONLINEPLAYER, MAXONLINEPLAYER, 1);
  }
}
*/
void setItemLore(Player* pl, vector<string>& lore) {
	auto item = pl->getSelectedItem();
	SymCall("?setCustomLore@ItemStackBase@@QEAAXAEBV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Z",
		void, void*, vector<string>)(&item, lore);
	SymCall("?sendInventory@ServerPlayer@@UEAAX_N@Z", void, void*, bool)(pl, true);
}
/*
void forEachDim(std::function<bool(Dimension*)> func) {
	SymCall("?forEachDimension@Level@@QEAAXV?$function@$$A6A_NAEAVDimension@@@Z@std@@@Z"
		, void, Level*, std::function<bool(Dimension*)>)(mc->getLevel(), func);
}
*/
void sendText(ServerPlayer* pl, string text, TextType tp) {
	Packet* pkt;
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		void*, Packet**, int)(&pkt, 9);//������
	dAccess<char, 48>(pkt) = (char)tp;
	dAccess<string, 56>(pkt) = u8"Server";
	//dAccess<string, 48>(pkt) = this->getName();
	dAccess<string, 88>(pkt) = text;
	pl->sendNetworkPacket(*pkt);
}