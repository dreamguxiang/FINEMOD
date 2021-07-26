// This header is Generate By WangYneos's Generator
// FileName:Container
// Class:Container
#include <mc/Core.h>
#include <api/MC.h>
class Container {
public:
	//void Container::addItem(ItemStack &)
	MCINLINE void addItem(ItemStack & arg0) {
		void (Container::*fnp)(ItemStack &)const;
		*((void **)&fnp) = dlsym("?addItem@Container@@UEAAXAEAVItemStack@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool Container::addItemToFirstEmptySlot(ItemStack &)
	MCINLINE bool addItemToFirstEmptySlot(ItemStack & arg0) {
		bool (Container::*fnp)(ItemStack &)const;
		*((void **)&fnp) = dlsym("?addItemToFirstEmptySlot@Container@@UEAA_NAEAVItemStack@@@Z");
		return (this->*fnp)(arg0);
	}


	//void Container::dropContents(BlockSource &,Vec3 &,bool)
	MCINLINE void dropContents(BlockSource & arg0,Vec3 & arg1,bool arg2) {
		void (Container::*fnp)(BlockSource &,Vec3 &,bool)const;
		*((void **)&fnp) = dlsym("?dropContents@Container@@UEAAXAEAVBlockSource@@AEBVVec3@@_N@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//int Container::findFirstSlotForItem(ItemStack &)
	MCINLINE int findFirstSlotForItem(ItemStack & arg0) {
		int (Container::*fnp)(ItemStack &)const;
		*((void **)&fnp) = dlsym("?findFirstSlotForItem@Container@@UEBAHAEBVItemStack@@@Z");
		return (this->*fnp)(arg0);
	}

	MCINLINE std::vector<ItemStack*, std::allocator<ItemStack*>>getSlots() {
		std::vector<ItemStack*, std::allocator<ItemStack*>>(Container:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ");
		return (this->*fnp)();
	}
	//int Container::getItemCount(ItemStack &)
	MCINLINE int getItemCount(ItemStack & arg0) {
		int (Container::*fnp)(ItemStack &)const;
		*((void **)&fnp) = dlsym("?getItemCount@Container@@UEBAHAEBVItemStack@@@Z");
		return (this->*fnp)(arg0);
	}

	//int Container::getRedstoneSignalFromContainer(BlockSource &)
	MCINLINE int getRedstoneSignalFromContainer(BlockSource & arg0) {
		int (Container::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?getRedstoneSignalFromContainer@Container@@QEAAHAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}

	//std::vector<ItemStack,std::allocator<ItemStack>>Container::getSlotCopies()
	MCINLINE std::vector<ItemStack,std::allocator<ItemStack>>getSlotCopies() {
		std::vector<ItemStack,std::allocator<ItemStack>>(Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?getSlotCopies@Container@@UEBA?AV?$vector@VItemStack@@V?$allocator@VItemStack@@@std@@@std@@XZ");
		return (this->*fnp)();
	}
	//std::vector<ItemStack *,std::allocator<ItemStack *>>Container::getSlots()
	MCINLINE std::vector<ItemStack *,std::allocator<ItemStack *>>getSlot() {
		std::vector<ItemStack *,std::allocator<ItemStack *>>(Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?getSlots@Container@@UEBA?BV?$vector@PEBVItemStack@@V?$allocator@PEBVItemStack@@@std@@@std@@XZ");
		return (this->*fnp)();
	}
	//bool Container::hasCustomName()
	MCINLINE bool hasCustomName() {
		bool (Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?hasCustomName@Container@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//bool Container::hasRoomForItem(ItemStack &)
	MCINLINE bool hasRoomForItem(ItemStack & arg0) {
		bool (Container::*fnp)(ItemStack &)const;
		*((void **)&fnp) = dlsym("?hasRoomForItem@Container@@UEAA_NAEBVItemStack@@@Z");
		return (this->*fnp)(arg0);
	}
	//void Container::initRuntimeId()
	MCINLINE void initRuntimeId() {
		void (Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?initRuntimeId@Container@@QEAAXXZ");
		return (this->*fnp)();
	}
	//bool Container::isEmpty()
	MCINLINE bool isEmpty() {
		bool (Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?isEmpty@Container@@UEBA_NXZ");
		return (this->*fnp)();
	}

	//void Container::removeAllItems()
	MCINLINE void removeAllItems() {
		void (Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?removeAllItems@Container@@UEAAXXZ");
		return (this->*fnp)();
	}

	//void Container::removeItem(int,int)
	MCINLINE void removeItem(int arg0,int arg1) {
		void (Container::*fnp)(int,int)const;
		*((void **)&fnp) = dlsym("?removeItem@Container@@UEAAXHH@Z");
		return (this->*fnp)(arg0,arg1);
	}

	//void Container::setContainerChanged(int)
	MCINLINE void setContainerChanged(int arg0) {
		void (Container::*fnp)(int)const;
		*((void **)&fnp) = dlsym("?setContainerChanged@Container@@UEAAXH@Z");
		return (this->*fnp)(arg0);
	}
	//void Container::setContainerMoved()
	MCINLINE void setContainerMoved() {
		void (Container::*fnp)()const;
		*((void **)&fnp) = dlsym("?setContainerMoved@Container@@UEAAXXZ");
		return (this->*fnp)();
	}
	//void Container::setCustomName(std::basic_string<char,std::char_traits<char>,std::allocator<char>>&)
	MCINLINE void setCustomName(std::basic_string<char,std::char_traits<char>,std::allocator<char>>& arg0) {
		void (Container::*fnp)(std::basic_string<char,std::char_traits<char>,std::allocator<char>>&)const;
		*((void **)&fnp) = dlsym("?setCustomName@Container@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
		return (this->*fnp)(arg0);
	}

	//void Container::setItemWithForceBalance(int,ItemStack &,bool)
	MCINLINE void setItemWithForceBalance(int arg0,ItemStack & arg1,bool arg2) {
		void (Container::*fnp)(int,ItemStack &,bool)const;
		*((void **)&fnp) = dlsym("?setItemWithForceBalance@Container@@UEAAXHAEBVItemStack@@_N@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//void Container::triggerTransactionChange(int,ItemStack &,ItemStack &)
	MCINLINE void triggerTransactionChange(int arg0,ItemStack & arg1,ItemStack & arg2) {
		void (Container::*fnp)(int,ItemStack &,ItemStack &)const;
		*((void **)&fnp) = dlsym("?triggerTransactionChange@Container@@QEAAXHAEBVItemStack@@0@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	// Container::~Container()
	MCINLINE ~Container() {
		void(Container::*fnp)()const;
		*((void **)&fnp) = dlsym("??1Container@@UEAA@XZ");
		(this->*fnp)();
	}
};
