// This header is Generate By WangYneos's Generator
// FileName:ChestBlockActor Class:ChestBlockActor
#include <mc/Core.h>
#include <api/MC.h>
#include "BlockActor.hpp"

class ChestBlockActor : public BlockActor {

public:
	// ChestBlockActor::ChestBlockActor(BlockActorType,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &,BlockActorRendererId,BlockPos const &,bool)
	/*
	MCINLINE ChestBlockActor(BlockActorType arg0,std::string const & arg1,BlockActorRendererId arg2,BlockPos const & arg3,bool arg4) {
		void(ChestBlockActor::*fnp)(BlockActorType,std::string const &,BlockActorRendererId,BlockPos const &,bool)const;
		*((void **)&fnp) = dlsym("??0ChestBlockActor@@QEAA@W4BlockActorType@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4BlockActorRendererId@@AEBVBlockPos@@_N@Z");
		(this->*fnp)(arg0,arg1,arg2,arg3,arg4);
	}*/
	//bool ChestBlockActor::_canOpenThis(BlockSource &)const 
	MCINLINE bool _canOpenThis(BlockSource & arg0) {
		bool (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?_canOpenThis@ChestBlockActor@@MEBA_NAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::_closeChest(BlockSource &,Player *)
	MCINLINE void _closeChest(BlockSource & arg0,Player * arg1) {
		void (ChestBlockActor::*fnp)(BlockSource &,Player *)const;
		*((void **)&fnp) = dlsym("?_closeChest@ChestBlockActor@@AEAAXAEAVBlockSource@@PEAVPlayer@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//bool ChestBlockActor::_detectEntityObstruction(BlockSource &)const 
	MCINLINE bool _detectEntityObstruction(BlockSource & arg0) {
		bool (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?_detectEntityObstruction@ChestBlockActor@@MEBA_NAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::_getCenter(float &,float &,float &)
	MCINLINE void _getCenter(float & arg0,float & arg1,float & arg2) {
		void (ChestBlockActor::*fnp)(float &,float &,float &)const;
		*((void **)&fnp) = dlsym("?_getCenter@ChestBlockActor@@IEAAXAEAM00@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//std::unique_ptr<BlockActorDataPacket,std::default_delete<BlockActorDataPacket> > ChestBlockActor::_getUpdatePacket(BlockSource &)

	//void ChestBlockActor::_onUpdatePacket(CompoundTag const &,BlockSource &)
	MCINLINE void _onUpdatePacket(class CompoundTag const & arg0,BlockSource & arg1) {
		void (ChestBlockActor::*fnp)(CompoundTag const &,BlockSource &)const;
		*((void **)&fnp) = dlsym("?_onUpdatePacket@ChestBlockActor@@MEAAXAEBVCompoundTag@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//bool ChestBlockActor::_saveClientSideState(CompoundTag &)const 
	MCINLINE bool _saveClientSideState(CompoundTag & arg0) {
		bool (ChestBlockActor::*fnp)(CompoundTag &)const;
		*((void **)&fnp) = dlsym("?_saveClientSideState@ChestBlockActor@@AEBA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::_tryToPairWith(BlockSource &,BlockPos const &)
	MCINLINE void _tryToPairWith(BlockSource & arg0,BlockPos const & arg1) {
		void (ChestBlockActor::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?_tryToPairWith@ChestBlockActor@@AEAAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::_unpair()
	MCINLINE void _unpair() {
		void (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?_unpair@ChestBlockActor@@AEAAXXZ");
		return (this->*fnp)();
	}
	//void ChestBlockActor::addContentChangeListener(ContainerContentChangeListener *)
	MCINLINE void addContentChangeListener(class ContainerContentChangeListener * arg0) {
		void (ChestBlockActor::*fnp)(ContainerContentChangeListener *)const;
		*((void **)&fnp) = dlsym("?addContentChangeListener@ChestBlockActor@@UEAAXPEAVContainerContentChangeListener@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::canOpen(BlockSource &)const 
	MCINLINE bool canOpen(BlockSource & arg0) {
		bool (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?canOpen@ChestBlockActor@@QEBA_NAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::canPairWith(BlockActor *,BlockSource &)
	MCINLINE bool canPairWith(BlockActor * arg0,BlockSource & arg1) {
		bool (ChestBlockActor::*fnp)(BlockActor *,BlockSource &)const;
		*((void **)&fnp) = dlsym("?canPairWith@ChestBlockActor@@QEAA_NPEAVBlockActor@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//bool ChestBlockActor::canPullOutItem(BlockSource &,int,int,ItemInstance const &)const 
	MCINLINE bool canPullOutItem(BlockSource & arg0,int arg1,int arg2,class ItemInstance const & arg3) {
		bool (ChestBlockActor::*fnp)(BlockSource &,int,int,ItemInstance const &)const;
		*((void **)&fnp) = dlsym("?canPullOutItem@ChestBlockActor@@UEBA_NAEAVBlockSource@@HHAEBVItemInstance@@@Z");
		return (this->*fnp)(arg0,arg1,arg2,arg3);
	}
	//bool ChestBlockActor::canPushInItem(BlockSource &,int,int,ItemInstance const &)const 
	MCINLINE bool canPushInItem(BlockSource & arg0,int arg1,int arg2,ItemInstance const & arg3) {
		bool (ChestBlockActor::*fnp)(BlockSource &,int,int,ItemInstance const &)const;
		*((void **)&fnp) = dlsym("?canPushInItem@ChestBlockActor@@UEBA_NAEAVBlockSource@@HHAEBVItemInstance@@@Z");
		return (this->*fnp)(arg0,arg1,arg2,arg3);
	}
	//void ChestBlockActor::clearCache()
	MCINLINE void clearCache() {
		void (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?clearCache@ChestBlockActor@@UEAAXXZ");
		return (this->*fnp)();
	}
	//int ChestBlockActor::clearInventory(int)
	MCINLINE int clearInventory(int arg0) {
		int (ChestBlockActor::*fnp)(int)const;
		*((void **)&fnp) = dlsym("?clearInventory@ChestBlockActor@@UEAAHH@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::forceCloseChest(BlockSource &)
	MCINLINE void forceCloseChest(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?forceCloseChest@ChestBlockActor@@QEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//Container * ChestBlockActor::getContainer()
	MCINLINE class Container * getContainer() {
		Container * (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getContainer@ChestBlockActor@@UEAAPEAVContainer@@XZ");
		return (this->*fnp)();
	}
	//Container const * ChestBlockActor::getContainer()const 
	/*
	MCINLINE class Container const * getContainer() {
		Container const * (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getContainer@ChestBlockActor@@UEBAPEBVContainer@@XZ");
		return (this->*fnp)();
	}*/
	//int ChestBlockActor::getContainerSize()const 
	MCINLINE int getContainerSize() {
		int (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getContainerSize@ChestBlockActor@@UEBAHXZ");
		return (this->*fnp)();
	}
	//BlockActor * ChestBlockActor::getCrackEntity(BlockSource &,BlockPos const &)
	MCINLINE BlockActor * getCrackEntity(BlockSource & arg0,BlockPos const & arg1) {
		BlockActor * (ChestBlockActor::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?getCrackEntity@ChestBlockActor@@UEAAPEAVBlockActor@@AEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::getDebugText(std::vector<std::basic_string<char,std::char_traits<char>,std::allocator<char> >,std::allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > > &,BlockPos const &)
	MCINLINE void getDebugText(std::vector<std::string,std::allocator<std::string > > & arg0,BlockPos const & arg1) {
		void (ChestBlockActor::*fnp)(std::vector<std::string,std::allocator<std::string > > &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?getDebugText@ChestBlockActor@@UEAAXAEAV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//ItemStack const & ChestBlockActor::getItem(int)const 
	MCINLINE ItemStack const & getItem(int arg0) {
		ItemStack const & (ChestBlockActor::*fnp)(int)const;
		*((void **)&fnp) = dlsym("?getItem@ChestBlockActor@@UEBAAEBVItemStack@@H@Z");
		return (this->*fnp)(arg0);
	}
	//int ChestBlockActor::getMaxStackSize()const 
	MCINLINE int getMaxStackSize() {
		int (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getMaxStackSize@ChestBlockActor@@UEBAHXZ");
		return (this->*fnp)();
	}
	//std::basic_string<char,std::char_traits<char>,std::allocator<char> > ChestBlockActor::getName()const 
	MCINLINE std::string getName() {
		std::string (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getName@ChestBlockActor@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
		return (this->*fnp)();
	}
	//AABB ChestBlockActor::getObstructionAABB()const 


	//int ChestBlockActor::getOpenCount()const 
	MCINLINE int getOpenCount() {
		int (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getOpenCount@ChestBlockActor@@QEBAHXZ");
		return (this->*fnp)();
	}
	//float ChestBlockActor::getOpenness()const 
	MCINLINE float getOpenness() {
		float (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getOpenness@ChestBlockActor@@QEBAMXZ");
		return (this->*fnp)();
	}
	//BlockPos const & ChestBlockActor::getPairedChestPosition()
	MCINLINE BlockPos const & getPairedChestPosition() {
		BlockPos const & (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?getPairedChestPosition@ChestBlockActor@@QEAAAEBVBlockPos@@XZ");
		return (this->*fnp)();
	}
	//void ChestBlockActor::initializeContainerContents(BlockSource &)
	MCINLINE void initializeContainerContents(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?initializeContainerContents@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::isFindable()const 
	MCINLINE bool isFindable() {
		bool (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?isFindable@ChestBlockActor@@QEBA_NXZ");
		return (this->*fnp)();
	}
	//bool ChestBlockActor::isLargeChest()const 
	MCINLINE bool isLargeChest() {
		bool (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?isLargeChest@ChestBlockActor@@QEBA_NXZ");
		return (this->*fnp)();
	}
	//void ChestBlockActor::load(Level &,CompoundTag const &,DataLoadHelper &)
	MCINLINE void load(Level & arg0,CompoundTag const & arg1,class DataLoadHelper & arg2) {
		void (ChestBlockActor::*fnp)(Level &,CompoundTag const &,DataLoadHelper &)const;
		*((void **)&fnp) = dlsym("?load@ChestBlockActor@@UEAAXAEAVLevel@@AEBVCompoundTag@@AEAVDataLoadHelper@@@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//void ChestBlockActor::loadItems(CompoundTag const &,Level &)
	MCINLINE void loadItems(CompoundTag const & arg0,Level & arg1) {
		void (ChestBlockActor::*fnp)(CompoundTag const &,Level &)const;
		*((void **)&fnp) = dlsym("?loadItems@ChestBlockActor@@UEAAXAEBVCompoundTag@@AEAVLevel@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::onChanged(BlockSource &)
	MCINLINE void onChanged(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?onChanged@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::onMove(BlockSource &,BlockPos const &,BlockPos const &)
	MCINLINE void onMove(BlockSource & arg0,BlockPos const & arg1,BlockPos const & arg2) {
		void (ChestBlockActor::*fnp)(BlockSource &,BlockPos const &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?onMove@ChestBlockActor@@QEAAXAEAVBlockSource@@AEBVBlockPos@@1@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//void ChestBlockActor::onMove()
	MCINLINE void onMove() {
		void (ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("?onMove@ChestBlockActor@@UEAAXXZ");
		return (this->*fnp)();
	}
	//void ChestBlockActor::onNeighborChanged(BlockSource &,BlockPos const &)
	MCINLINE void onNeighborChanged(BlockSource & arg0,BlockPos const & arg1) {
		void (ChestBlockActor::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?onNeighborChanged@ChestBlockActor@@UEAAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::onPlace(BlockSource &)
	MCINLINE void onPlace(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?onPlace@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::onRemoved(BlockSource &)
	MCINLINE void onRemoved(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?onRemoved@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::openBy(Player &)
	MCINLINE void openBy(Player & arg0) {
		void (ChestBlockActor::*fnp)(Player &)const;
		*((void **)&fnp) = dlsym("?openBy@ChestBlockActor@@QEAAXAEAVPlayer@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::pairWith(ChestBlockActor *,bool)
	MCINLINE void pairWith(ChestBlockActor * arg0,bool arg1) {
		void (ChestBlockActor::*fnp)(ChestBlockActor *,bool)const;
		*((void **)&fnp) = dlsym("?pairWith@ChestBlockActor@@QEAAXPEAV1@_N@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::playCloseSound(BlockSource &)
	MCINLINE void playCloseSound(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?playCloseSound@ChestBlockActor@@MEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::playOpenSound(BlockSource &)
	MCINLINE void playOpenSound(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?playOpenSound@ChestBlockActor@@MEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::removeContentChangeListener(ContainerContentChangeListener *)
	MCINLINE void removeContentChangeListener(ContainerContentChangeListener * arg0) {
		void (ChestBlockActor::*fnp)(ContainerContentChangeListener *)const;
		*((void **)&fnp) = dlsym("?removeContentChangeListener@ChestBlockActor@@UEAAXPEAVContainerContentChangeListener@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::save(CompoundTag &)const 
	MCINLINE bool save(CompoundTag & arg0) {
		bool (ChestBlockActor::*fnp)(CompoundTag &)const;
		*((void **)&fnp) = dlsym("?save@ChestBlockActor@@UEBA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::saveItemInstanceData(CompoundTag &)
	MCINLINE bool saveItemInstanceData(CompoundTag & arg0) {
		bool (ChestBlockActor::*fnp)(CompoundTag &)const;
		*((void **)&fnp) = dlsym("?saveItemInstanceData@ChestBlockActor@@UEAA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool ChestBlockActor::saveItems(CompoundTag &)const 
	MCINLINE bool saveItems(CompoundTag & arg0) {
		bool (ChestBlockActor::*fnp)(CompoundTag &)const;
		*((void **)&fnp) = dlsym("?saveItems@ChestBlockActor@@UEBA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::serverInitItemStackIds(int,int,std::function<void (int,ItemStack const &)>)

	//void ChestBlockActor::setContainerChanged(int)
	MCINLINE void setContainerChanged(int arg0) {
		void (ChestBlockActor::*fnp)(int)const;
		*((void **)&fnp) = dlsym("?setContainerChanged@ChestBlockActor@@UEAAXH@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::setFindable(bool)
	MCINLINE void setFindable(bool arg0) {
		void (ChestBlockActor::*fnp)(bool)const;
		*((void **)&fnp) = dlsym("?setFindable@ChestBlockActor@@QEAAX_N@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::setItem(int,ItemStack const &)
	MCINLINE void setItem(int arg0,ItemStack const & arg1) {
		void (ChestBlockActor::*fnp)(int,ItemStack const &)const;
		*((void **)&fnp) = dlsym("?setItem@ChestBlockActor@@UEAAXHAEBVItemStack@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::startOpen(Player &)
	MCINLINE void startOpen(Player & arg0) {
		void (ChestBlockActor::*fnp)(Player &)const;
		*((void **)&fnp) = dlsym("?startOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::stopOpen(Player &)
	MCINLINE void stopOpen(Player & arg0) {
		void (ChestBlockActor::*fnp)(Player &)const;
		*((void **)&fnp) = dlsym("?stopOpen@ChestBlockActor@@UEAAXAEAVPlayer@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::tick(BlockSource &)
	MCINLINE void tick(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?tick@ChestBlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void ChestBlockActor::triggerEvent(int,int)
	MCINLINE void triggerEvent(int arg0,int arg1) {
		void (ChestBlockActor::*fnp)(int,int)const;
		*((void **)&fnp) = dlsym("?triggerEvent@ChestBlockActor@@UEAAXHH@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlockActor::unpair(BlockSource &)
	MCINLINE void unpair(BlockSource & arg0) {
		void (ChestBlockActor::*fnp)(BlockSource &)const;
		*((void **)&fnp) = dlsym("?unpair@ChestBlockActor@@QEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	// ChestBlockActor::~ChestBlockActor()
	MCINLINE ~ChestBlockActor() {
		void(ChestBlockActor::*fnp)()const;
		*((void **)&fnp) = dlsym("??1ChestBlockActor@@UEAA@XZ");
		(this->*fnp)();
	}
	//BlockPos* getPosition() {//BlockActor::BlockActor 23
	//	return (BlockPos*)(this + 44);
	//}
};
