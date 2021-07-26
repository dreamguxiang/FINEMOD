#include <iostream>
#include <lbpch.h>
#include "pch.h"
#include <api/MC.h>
#include <mc/Player.h>
#include <rapidjson/document.h>
#include<streambuf>
#include<api\regCommandHelper.h>
#include<api/types/types.h>

bool kg;
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
namespace offItemStack {
	inline int getCount(ItemStack* a1) {
		return *((char*)a1 + 34);
		//LevelContainerModel::_getContainer
	}
}
using namespace std;
THook(bool, "?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* _this,  ItemStack* item, BlockPos* a3, unsigned __int8 a4,  Vec3* a5, Block* a6) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(_this) + 8);
			string mstr2 = "";
			string mstr3 = "";
			auto offitem = SymCall("?getOffhandSlot@Actor@@QEBAAEBVItemStack@@XZ", ItemStack*, Actor*)(sp);
			if (offitem) {
				SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(offitem->getItem(), &mstr3);
				std::cout << u8"[Id-Test]使用触发者:" << sp->getNameTag() << u8"|副手物品名称:" << mstr3 << u8"|物品id:" << offitem->getId() << u8"|物品数量:" << offItemStack::getCount(offitem) << endl;
			}
			if (item) {
				SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(item->getItem(), &mstr2);
				std::cout << u8"[Id-Test]使用触发者:" << sp->getNameTag() << u8"|物品名称:" << mstr2 << u8"|物品id:" << item->getId() << u8"|物品数量:" << offItemStack::getCount(item) << endl;
			}
	original(_this, item,a3,a4,a5,a6);
}

THook(bool, "?attack@GameMode@@UEAA_NAEAVActor@@@Z", void* _this, Actor* ac) {
	if (ac) {
		string name;
		auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(_this) + 8);
			name = SymCall("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z", string, int, int)(ac->getEntityTypeId(), 1);
			std::cout << u8"[Id-Test]使用触发者:" << sp->getNameTag() << u8"|生物名称:" << name << u8"|生物id:" << ac->getEntityTypeId() << endl;
	}
	return original(_this, ac);
}



void entry() {

}
