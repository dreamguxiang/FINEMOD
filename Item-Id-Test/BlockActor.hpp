// This header is Generate By WangYneos's Generator
// FileName:BlockActor Class:BlockActor
#include <mc/Core.h>
#include <api/MC.h>

class BlockActor {
public:
	//std::map<std::basic_string<char,std::char_traits<char>,std::allocator<char> >,BlockActorType,std::less<std::basic_string<char,std::char_traits<char>,std::allocator<char> > >,std::allocator<std::pair<std::basic_string<char,std::char_traits<char>,std::allocator<char> > const ,BlockActorType> > > BlockActor::_createIdClassMap()

	//std::unique_ptr<BlockActorDataPacket,std::default_delete<BlockActorDataPacket> > BlockActor::_getUpdatePacket(BlockSource &)

	//void BlockActor::_onUpdatePacket(CompoundTag const &,BlockSource &)
	MCINLINE void _onUpdatePacket(class CompoundTag const& arg0, BlockSource& arg1) {
		void (BlockActor:: * fnp)(CompoundTag const&, BlockSource&)const;
		*((void**)&fnp) = dlsym("?_onUpdatePacket@BlockActor@@MEAAXAEBVCompoundTag@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0, arg1);
	}
	//bool BlockActor::_playerCanUpdate(Player const &)const 
	MCINLINE bool _playerCanUpdate(Player const& arg0) {
		bool (BlockActor:: * fnp)(Player const&)const;
		*((void**)&fnp) = dlsym("?_playerCanUpdate@BlockActor@@MEBA_NAEBVPlayer@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::_resetAABB()
	MCINLINE void _resetAABB() {
		void (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?_resetAABB@BlockActor@@IEAAXXZ");
		return (this->*fnp)();
	}
	//void BlockActor::assignBlockIfNotAssigned(BlockSource &)
	MCINLINE void assignBlockIfNotAssigned(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?assignBlockIfNotAssigned@BlockActor@@QEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::clearCache()
	MCINLINE void clearCache() {
		void (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?clearCache@BlockActor@@UEAAXXZ");
		return (this->*fnp)();
	}
	//AABB const & BlockActor::getAABB()const 
	MCINLINE class AABB const& getAABB() {
		AABB const& (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getAABB@BlockActor@@QEBAAEBVAABB@@XZ");
		return (this->*fnp)();
	}
	//Block const * BlockActor::getBlock()const 
	MCINLINE Block const* getBlock() {
		Block const* (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getBlock@BlockActor@@QEBAPEBVBlock@@XZ");
		return (this->*fnp)();
	}
	//Container const * BlockActor::getContainer()const 
	MCINLINE class Container const* getContainer() {
		Container const* (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getContainer@BlockActor@@UEBAPEBVContainer@@XZ");
		return (this->*fnp)();
	}
	//Container * BlockActor::getContainer()
	//BlockActor * BlockActor::getCrackEntity(BlockSource &,BlockPos const &)
	MCINLINE BlockActor* getCrackEntity(BlockSource& arg0, BlockPos const& arg1) {
		BlockActor* (BlockActor:: * fnp)(BlockSource&, BlockPos const&)const;
		*((void**)&fnp) = dlsym("?getCrackEntity@BlockActor@@UEAAPEAV1@AEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0, arg1);
	}
	//std::basic_string<char,std::char_traits<char>,std::allocator<char> > const & BlockActor::getCustomName()const 
	MCINLINE std::string const& getCustomName() {
		std::string const& (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getCustomName@BlockActor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
		return (this->*fnp)();
	}
	//void BlockActor::getDebugText(std::vector<std::basic_string<char,std::char_traits<char>,std::allocator<char> >,std::allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > > &,BlockPos const &)
	MCINLINE void getDebugText(std::vector<std::string, std::allocator<std::string > >& arg0, BlockPos const& arg1) {
		void (BlockActor:: * fnp)(std::vector<std::string, std::allocator<std::string > > &, BlockPos const&)const;
		*((void**)&fnp) = dlsym("?getDebugText@BlockActor@@UEAAXAEAV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0, arg1);
	}
	//float BlockActor::getDeletionDelayTimeSeconds()const 
	MCINLINE float getDeletionDelayTimeSeconds() {
		float (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getDeletionDelayTimeSeconds@BlockActor@@UEBAMXZ");
		return (this->*fnp)();
	}
	//std::basic_string<char,std::char_traits<char>,std::allocator<char> > const & BlockActor::getFilteredCustomName(UIProfanityContext const &)
	MCINLINE std::string const& getFilteredCustomName(class UIProfanityContext const& arg0) {
		std::string const& (BlockActor:: * fnp)(UIProfanityContext const&)const;
		*((void**)&fnp) = dlsym("?getFilteredCustomName@BlockActor@@UEAAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUIProfanityContext@@@Z");
		return (this->*fnp)(arg0);
	}
	//std::basic_string<char,std::char_traits<char>,std::allocator<char> > BlockActor::getImmersiveReaderText(BlockSource &)
	MCINLINE std::string getImmersiveReaderText(BlockSource& arg0) {
		std::string(BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?getImmersiveReaderText@BlockActor@@UEAA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//std::basic_string<char,std::char_traits<char>,std::allocator<char> > BlockActor::getName()const 
	MCINLINE std::string getName() {
		std::string(BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getName@BlockActor@@UEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
		return (this->*fnp)();
	}
	//PistonBlockActor * BlockActor::getOwningPiston(BlockSource &)
	MCINLINE class PistonBlockActor* getOwningPiston(BlockSource& arg0) {
		PistonBlockActor* (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?getOwningPiston@BlockActor@@UEAAPEAVPistonBlockActor@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//BlockPos const & BlockActor::getPosition()const 
	MCINLINE BlockPos const& getPosition() {
		BlockPos const& (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getPosition@BlockActor@@QEBAAEBVBlockPos@@XZ");
		return (this->*fnp)();
	}
	//int BlockActor::getRepairCost()const 
	MCINLINE int getRepairCost() {
		int (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getRepairCost@BlockActor@@UEBAHXZ");
		return (this->*fnp)();
	}
	//std::unique_ptr<BlockActorDataPacket,std::default_delete<BlockActorDataPacket> > BlockActor::getServerUpdatePacket(BlockSource &)

	//float BlockActor::getShadowRadius(BlockSource &)const 
	MCINLINE float getShadowRadius(BlockSource& arg0) {
		float (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?getShadowRadius@BlockActor@@UEBAMAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//BlockActorType const & BlockActor::getType()const 
	MCINLINE class BlockActorType const& getType() {
		BlockActorType const& (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?getType@BlockActor@@QEBAAEBW4BlockActorType@@XZ");
		return (this->*fnp)();
	}
	//bool BlockActor::hasAlphaLayer()const 
	MCINLINE bool hasAlphaLayer() {
		bool (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?hasAlphaLayer@BlockActor@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//bool const BlockActor::ignoreLighting()const 
	MCINLINE bool const ignoreLighting() {
		bool const (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?ignoreLighting@BlockActor@@QEBA?B_NXZ");
		return (this->*fnp)();
	}
	//bool BlockActor::isCustomNameSaved()
	MCINLINE bool isCustomNameSaved() {
		bool (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?isCustomNameSaved@BlockActor@@UEAA_NXZ");
		return (this->*fnp)();
	}
	//bool BlockActor::isFinished()
	MCINLINE bool isFinished() {
		bool (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?isFinished@BlockActor@@UEAA_NXZ");
		return (this->*fnp)();
	}
	//bool BlockActor::isMovable(BlockSource &)
	MCINLINE bool isMovable(BlockSource& arg0) {
		bool (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?isMovable@BlockActor@@UEAA_NAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}

	//void BlockActor::load(Level &,CompoundTag const &,DataLoadHelper &)
	MCINLINE void load(Level& arg0, CompoundTag const& arg1, class DataLoadHelper& arg2) {
		void (BlockActor:: * fnp)(Level&, CompoundTag const&, DataLoadHelper&)const;
		*((void**)&fnp) = dlsym("?load@BlockActor@@UEAAXAEAVLevel@@AEBVCompoundTag@@AEAVDataLoadHelper@@@Z");
		return (this->*fnp)(arg0, arg1, arg2);
	}
	//void BlockActor::loadBlockData(CompoundTag const &,BlockSource &,DataLoadHelper &)
	MCINLINE void loadBlockData(CompoundTag const& arg0, BlockSource& arg1, DataLoadHelper& arg2) {
		void (BlockActor:: * fnp)(CompoundTag const&, BlockSource&, DataLoadHelper&)const;
		*((void**)&fnp) = dlsym("?loadBlockData@BlockActor@@UEAAXAEBVCompoundTag@@AEAVBlockSource@@AEAVDataLoadHelper@@@Z");
		return (this->*fnp)(arg0, arg1, arg2);
	}
	//std::shared_ptr<BlockActor> BlockActor::loadStatic(Level &,CompoundTag const &,DataLoadHelper &)
	MCINLINE std::shared_ptr<BlockActor> loadStatic(Level& arg0, CompoundTag const& arg1, DataLoadHelper& arg2) {
		std::shared_ptr<BlockActor>(BlockActor:: * fnp)(Level&, CompoundTag const&, DataLoadHelper&)const;
		*((void**)&fnp) = dlsym("?loadStatic@BlockActor@@SA?AV?$shared_ptr@VBlockActor@@@std@@AEAVLevel@@AEBVCompoundTag@@AEAVDataLoadHelper@@@Z");
		return (this->*fnp)(arg0, arg1, arg2);
	}

	//void BlockActor::moveTo(BlockPos const &)
	MCINLINE void moveTo(BlockPos const& arg0) {
		void (BlockActor:: * fnp)(BlockPos const&)const;
		*((void**)&fnp) = dlsym("?moveTo@BlockActor@@QEAAXAEBVBlockPos@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onChanged(BlockSource &)
	MCINLINE void onChanged(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?onChanged@BlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onChunkLoaded(LevelChunk &)
	MCINLINE void onChunkLoaded(class LevelChunk& arg0) {
		void (BlockActor:: * fnp)(LevelChunk&)const;
		*((void**)&fnp) = dlsym("?onChunkLoaded@BlockActor@@UEAAXAEAVLevelChunk@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onChunkUnloaded(LevelChunk &)
	MCINLINE void onChunkUnloaded(LevelChunk& arg0) {
		void (BlockActor:: * fnp)(LevelChunk&)const;
		*((void**)&fnp) = dlsym("?onChunkUnloaded@BlockActor@@UEAAXAEAVLevelChunk@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onCustomTagLoadDone(BlockSource &)
	MCINLINE void onCustomTagLoadDone(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?onCustomTagLoadDone@BlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onMove()
	MCINLINE void onMove() {
		void (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?onMove@BlockActor@@UEAAXXZ");
		return (this->*fnp)();
	}
	//void BlockActor::onNeighborChanged(BlockSource &,BlockPos const &)
	MCINLINE void onNeighborChanged(BlockSource& arg0, BlockPos const& arg1) {
		void (BlockActor:: * fnp)(BlockSource&, BlockPos const&)const;
		*((void**)&fnp) = dlsym("?onNeighborChanged@BlockActor@@UEAAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0, arg1);
	}
	//void BlockActor::onPlace(BlockSource &)
	MCINLINE void onPlace(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?onPlace@BlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::onRemoved(BlockSource &)
	MCINLINE void onRemoved(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?onRemoved@BlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//bool BlockActor::onUpdatePacket(CompoundTag const &,BlockSource &,Player const *)
	MCINLINE bool onUpdatePacket(CompoundTag const& arg0, BlockSource& arg1, Player const* arg2) {
		bool (BlockActor:: * fnp)(CompoundTag const&, BlockSource&, Player const*)const;
		*((void**)&fnp) = dlsym("?onUpdatePacket@BlockActor@@QEAA_NAEBVCompoundTag@@AEAVBlockSource@@PEBVPlayer@@@Z");
		return (this->*fnp)(arg0, arg1, arg2);
	}
	//bool BlockActor::save(CompoundTag &)const 
	MCINLINE bool save(CompoundTag& arg0) {
		bool (BlockActor:: * fnp)(CompoundTag&)const;
		*((void**)&fnp) = dlsym("?save@BlockActor@@UEBA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::saveBlockData(CompoundTag &,BlockSource &)const 
	MCINLINE void saveBlockData(CompoundTag& arg0, BlockSource& arg1) {
		void (BlockActor:: * fnp)(CompoundTag&, BlockSource&)const;
		*((void**)&fnp) = dlsym("?saveBlockData@BlockActor@@UEBAXAEAVCompoundTag@@AEAVBlockSource@@@Z");
		return (this->*fnp)(arg0, arg1);
	}
	//bool BlockActor::saveItemInstanceData(CompoundTag &)
	MCINLINE bool saveItemInstanceData(CompoundTag& arg0) {
		bool (BlockActor:: * fnp)(CompoundTag&)const;
		*((void**)&fnp) = dlsym("?saveItemInstanceData@BlockActor@@UEAA_NAEAVCompoundTag@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::setChanged()
	MCINLINE void setChanged() {
		void (BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("?setChanged@BlockActor@@QEAAXXZ");
		return (this->*fnp)();
	}
	//void BlockActor::setCustomName(std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &)
	MCINLINE void setCustomName(std::string const& arg0) {
		void (BlockActor:: * fnp)(std::string const&)const;
		*((void**)&fnp) = dlsym("?setCustomName@BlockActor@@UEAAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::setCustomNameSaved(bool)
	MCINLINE void setCustomNameSaved(bool arg0) {
		void (BlockActor:: * fnp)(bool)const;
		*((void**)&fnp) = dlsym("?setCustomNameSaved@BlockActor@@QEAAX_N@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::setIgnoreLighting(bool)
	MCINLINE void setIgnoreLighting(bool arg0) {
		void (BlockActor:: * fnp)(bool)const;
		*((void**)&fnp) = dlsym("?setIgnoreLighting@BlockActor@@QEAAX_N@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::setMovable(bool)
	MCINLINE void setMovable(bool arg0) {
		void (BlockActor:: * fnp)(bool)const;
		*((void**)&fnp) = dlsym("?setMovable@BlockActor@@QEAAX_N@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::tick(BlockSource &)
	MCINLINE void tick(BlockSource& arg0) {
		void (BlockActor:: * fnp)(BlockSource&)const;
		*((void**)&fnp) = dlsym("?tick@BlockActor@@UEAAXAEAVBlockSource@@@Z");
		return (this->*fnp)(arg0);
	}
	//void BlockActor::triggerEvent(int,int)
	MCINLINE void triggerEvent(int arg0, int arg1) {
		void (BlockActor:: * fnp)(int, int)const;
		*((void**)&fnp) = dlsym("?triggerEvent@BlockActor@@UEAAXHH@Z");
		return (this->*fnp)(arg0, arg1);
	}
	// BlockActor::~BlockActor()
	MCINLINE ~BlockActor() {
		void(BlockActor:: * fnp)()const;
		*((void**)&fnp) = dlsym("??1BlockActor@@UEAA@XZ");
		(this->*fnp)();
	}
};
#pragma once
