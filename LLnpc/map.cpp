#include"pch.h"
#include<mc/Level.h>

class LevelStorage;
bool onCMD_map(CommandOrigin const& ori, CommandOutput& outp, string& name) {
	auto wp = MakeWP(ori);
	auto sp = MakeSP(ori);
	auto& ite=wp.val()->getSelectedItem();
	auto level = offPlayer::getLevel(sp);
	//static auto map_vtable = SYM("??_7MapItem@@6B@");
	//printf("%p %p\n", map_vtable, dAccess<void*, 0>(&ite));
	ActorUniqueID id;
	void* tmp = nullptr;
	SymCall("?getMapId@MapItem@@SA?AUActorUniqueID@@PEBVCompoundTag@@@Z", void, ActorUniqueID*, void*,void*)(
		&id, *(void**)((uintptr_t)&ite + 16),tmp);
	auto mapd = level->getMapSavedData(id);
	printf("map %p\n", mapd);
	if (!mapd) { outp.error("not a filled map"); return false; }
	dAccess<bool, 0x63>(mapd) = true; //locked
	auto& pix = dAccess<std::vector<unsigned int>, 0x30>(mapd);
	printf("mid %I64u %zu\n", id.get(),pix.size());
	ifstream ifs(name + ".bin", std::ios::binary);
	auto str=ifs2str(ifs);
	printf("size %zd\n",str.size()/sizeof(unsigned int)/128);
	RBStream rs{ str };
	if (ifs.fail()) {
		outp.error("file open error");
		return false;
	}
	for (int i = 0; i < 16384; ++i) {
		unsigned int val;
		rs.apply(val);
		//printf("%p\n", val);
		pix[i] = 0xff000000|val;
	}
	for (int i = 0; i < 128; ++i)
		for (int j = 0; j < 128; ++j)
			SymCall("?setPixelDirty@MapItemSavedData@@QEAAXII@Z",void,void*, unsigned int, unsigned int)(mapd,i, j);
	auto ls = SymCall("?getLevelStorage@Level@@UEBAAEBVLevelStorage@@XZ", LevelStorage*, Level*)(level);
	SymCall("?save@MapItemSavedData@@QEAAXAEAVLevelStorage@@@Z", void, void*, void*)(mapd, ls);
	return true;
}
void entry_map() {
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		MakeCommand("map", "map maker", 1);
		CmdOverload(map, onCMD_map, "filename");
	});
}
