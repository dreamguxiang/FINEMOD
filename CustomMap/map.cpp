#include"pch.h"
#include<mc/Level.h>
#include <io.h>
class LevelStorage;
LevelStorage* ls;
string lite = "Unkown";
void ve() {
	if (_access("LiteLoader.dll", 00) == 0)
	{
		lite = "LiteLoader";
		return;
	}
	if (_access("BDXCORE.dll", 00) == 0)
	{
		lite = "BDXCore";
		return;
	}
	if (_access("lightbase.dll", 00) == 0)
	{
		lite = "bedrockx";
		return;
	}
}
bool onCMD_map(CommandOrigin const& ori, CommandOutput& outp, string& name) {
	auto wp = MakeWP(ori);
	auto sp = MakeSP(ori);
	auto& ite =wp.val()->getCarriedItem();
	auto level = offPlayer::getLevel(sp);
	//static auto map_vtable = SYM("??_7MapItem@@6B@");
	//printf("%p %p\n", map_vtable, dAccess<void*, 0>(&ite));
	ActorUniqueID id;
	void* tmp = nullptr;
	SymCall("?getMapId@MapItem@@SA?AUActorUniqueID@@PEBVCompoundTag@@@Z", void, ActorUniqueID*, uintptr_t ,void*)(
		&id, *(uintptr_t*)((uintptr_t)&ite + 16),tmp);
	auto mapd = level->getMapSavedData(id);
	//printf("map %p\n", mapd);
	if (!mapd) { outp.error(u8"¡ì6[¡ìeCustomMap¡ì6]¡ìr not a filled map"); return false; }
	dAccess<bool, 0x63>(mapd) = true; //locked
	auto& pix = dAccess<std::vector<unsigned int>, 0x30>(mapd);
	//printf("mid %I64u %zu\n", id.get(),pix.size());
	if (lite == "LiteLoader") {
		ifstream ifs("plugins/CustomMap/mapbin/" + name, std::ios::binary);
		auto str = ifs2str(ifs);
		//printf("size %zd\n",str.size()/sizeof(unsigned int)/128);
		RBStream rs{ str };
		if (ifs.fail()) {
			outp.error(u8"¡ì6[¡ìeCustomMap¡ì6]¡ìr file open error");
			return false;
		}
		for (int i = 0; i < 16384; ++i) {
			unsigned int val;
			rs.apply(val);
			//printf("%p\n", val);
			pix[i] = 0xff000000 | val;
		}
		for (int i = 0; i < 128; ++i)
			for (int j = 0; j < 128; ++j)
				SymCall("?setPixelDirty@MapItemSavedData@@QEAAXII@Z", void, void*, unsigned int, unsigned int)(mapd, i, j);
		//auto ls = SymCall("?getLevelStorage@Level@@UEBAAEBVLevelStorage@@XZ", LevelStorage*, Level*)(level);
		SymCall("?save@MapItemSavedData@@QEAAXAEAVLevelStorage@@@Z", void, void*, void*)(mapd, ls);
		outp.success(u8"¡ì6[¡ìeCustomMap¡ì6]¡ìr set map success");
		return true;
	}
	else {
		ifstream ifs("CustomMap/mapbin/" + name, std::ios::binary);
		auto str = ifs2str(ifs);
		//printf("size %zd\n",str.size()/sizeof(unsigned int)/128);
		RBStream rs{ str };
		if (ifs.fail()) {
			outp.error(u8"¡ì6[¡ìeCustomMap¡ì6]¡ìr file open error");
			return false;
		}
		for (int i = 0; i < 16384; ++i) {
			unsigned int val;
			rs.apply(val);
			//printf("%p\n", val);
			pix[i] = 0xff000000 | val;
		}
		for (int i = 0; i < 128; ++i)
			for (int j = 0; j < 128; ++j)
				SymCall("?setPixelDirty@MapItemSavedData@@QEAAXII@Z", void, void*, unsigned int, unsigned int)(mapd, i, j);
		//auto ls = SymCall("?getLevelStorage@Level@@UEBAAEBVLevelStorage@@XZ", LevelStorage*, Level*)(level);
		SymCall("?save@MapItemSavedData@@QEAAXAEAVLevelStorage@@@Z", void, void*, void*)(mapd, ls);
		outp.success(u8"¡ì6[¡ìeCustomMap¡ì6]¡ìr set map success");
		return true;
	}
}

using namespace std::filesystem;
void entry_map() {
	ve();
	std::cout << "   _____          _                  __  __             " << std::endl
		<< " / ____|        | |                |  \\/  |            " << std::endl
		<< "| |    _   _ ___| |_ ___  _ __ ___ | \\  / | __ _ _ __  " << std::endl
		<< "| |   | | | / __| __/ _ \\| '_ ` _ \\| |\\/| |/ _` | '_ \ " << std::endl
		<< "| |___| |_| \\__ \\ || (_) | | | | | | |  | | (_| | |_) |" << std::endl
		<< " \\_____\\__,_|___/\\__\\___/|_| |_| |_|_|  |_|\\__,_| .__/ " << std::endl
		<< "                                                | |    " << std::endl
		<< "                                                |_|    " << std::endl
		<< "[CustomMap] CustomMap For " << lite << " Loaded,By DreamGuXiang,WangYneos\n"
		<< "[CustomMap] Target: 1.16.210.06  Build Date [" << __TIMESTAMP__ << u8"] @BDXNPC(Sysca11)" << std::endl;
	if (lite == "LiteLoader") {
		create_directory("plugins\\CustomMap");
		create_directory("plugins\\CustomMap\\mapbin");
	}
	else {
		create_directory("CustomMap");
		create_directory("CustomMap\\mapbin");
	}
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("map", "map maker", 1);
		CmdOverload(map, onCMD_map, "filename");
	});

}

THook(void*, "?loadServerPlayerData@LevelStorage@@QEAA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@AEBVPlayer@@_N@Z", LevelStorage* a, void* b, void* c, void* d) {
	static bool inited = false;
	if (!inited) {
		ls = a;
		inited = true;
	}
	return original(a, b, c, d);
}