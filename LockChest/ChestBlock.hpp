// This header is Generate By WangYneos's Generator
// FileName:ChestBlock Class:ChestBlock
#include <mc/Core.h>

class ChestBlock
{
  public:
	// ChestBlock::ChestBlock(std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &,int,ChestBlock::ChestType,MaterialType)

	//bool ChestBlock::detachesOnPistonMove(BlockSource &,BlockPos const &)const 
	MCINLINE bool detachesOnPistonMove(BlockSource & arg0,BlockPos const & arg1) {
		bool (ChestBlock::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?detachesOnPistonMove@ChestBlock@@UEBA_NAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//int ChestBlock::getComparatorSignal(BlockSource &,BlockPos const &,Block const &,unsigned char)const 
	MCINLINE int getComparatorSignal(BlockSource & arg0,BlockPos const & arg1,Block const & arg2,unsigned char arg3) {
		int (ChestBlock::*fnp)(BlockSource &,BlockPos const &,Block const &,unsigned char)const;
		*((void **)&fnp) = dlsym("?getComparatorSignal@ChestBlock@@UEBAHAEAVBlockSource@@AEBVBlockPos@@AEBVBlock@@E@Z");
		return (this->*fnp)(arg0,arg1,arg2,arg3);
	}
	//unsigned char ChestBlock::getMappedFace(unsigned char,Block const &)const 
	MCINLINE unsigned char getMappedFace(unsigned char arg0,Block const & arg1) {
		unsigned char (ChestBlock::*fnp)(unsigned char,Block const &)const;
		*((void **)&fnp) = dlsym("?getMappedFace@ChestBlock@@UEBAEEAEBVBlock@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//Block const & ChestBlock::getPlacementBlock(Actor &,BlockPos const &,unsigned char,Vec3 const &,int)const 
	MCINLINE Block const & getPlacementBlock(Actor & arg0,BlockPos const & arg1,unsigned char arg2,Vec3 const & arg3,int arg4) {
		Block const & (ChestBlock::*fnp)(Actor &,BlockPos const &,unsigned char,Vec3 const &,int)const;
		*((void **)&fnp) = dlsym("?getPlacementBlock@ChestBlock@@UEBAAEBVBlock@@AEAVActor@@AEBVBlockPos@@EAEBVVec3@@H@Z");
		return (this->*fnp)(arg0,arg1,arg2,arg3,arg4);
	}
	//bool ChestBlock::getSecondPart(BlockSource &,BlockPos const &,BlockPos &)const 
	MCINLINE bool getSecondPart(BlockSource & arg0,BlockPos const & arg1,BlockPos & arg2) {
		bool (ChestBlock::*fnp)(BlockSource &,BlockPos const &,BlockPos &)const;
		*((void **)&fnp) = dlsym("?getSecondPart@ChestBlock@@UEBA_NAEAVBlockSource@@AEBVBlockPos@@AEAV3@@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//bool ChestBlock::hasComparatorSignal()const 
	MCINLINE bool hasComparatorSignal() {
		bool (ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("?hasComparatorSignal@ChestBlock@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//BlockLegacy & ChestBlock::init()
	MCINLINE BlockLegacy & init() {
		BlockLegacy & (ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("?init@ChestBlock@@UEAAAEAVBlockLegacy@@XZ");
		return (this->*fnp)();
	}
	//bool ChestBlock::isContainerBlock()const 
	MCINLINE bool isContainerBlock() {
		bool (ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("?isContainerBlock@ChestBlock@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//bool ChestBlock::isInteractiveBlock()const 
	MCINLINE bool isInteractiveBlock() {
		bool (ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("?isInteractiveBlock@ChestBlock@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//bool ChestBlock::isSignalSource()const 
	MCINLINE bool isSignalSource() {
		bool (ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("?isSignalSource@ChestBlock@@UEBA_NXZ");
		return (this->*fnp)();
	}
	//void ChestBlock::onMove(BlockSource &,BlockPos const &,BlockPos const &)const 
	MCINLINE void onMove(BlockSource & arg0,BlockPos const & arg1,BlockPos const & arg2) {
		void (ChestBlock::*fnp)(BlockSource &,BlockPos const &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?onMove@ChestBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@1@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//void ChestBlock::onPlace(BlockSource &,BlockPos const &)const 
	MCINLINE void onPlace(BlockSource & arg0,BlockPos const & arg1) {
		void (ChestBlock::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?onPlace@ChestBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlock::onRemove(BlockSource &,BlockPos const &)const 
	MCINLINE void onRemove(BlockSource & arg0,BlockPos const & arg1) {
		void (ChestBlock::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?onRemove@ChestBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//void ChestBlock::setupRedstoneComponent(BlockSource &,BlockPos const &)const 
	MCINLINE void setupRedstoneComponent(BlockSource & arg0,BlockPos const & arg1) {
		void (ChestBlock::*fnp)(BlockSource &,BlockPos const &)const;
		*((void **)&fnp) = dlsym("?setupRedstoneComponent@ChestBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@@Z");
		return (this->*fnp)(arg0,arg1);
	}
	//bool ChestBlock::shouldConnectToRedstone(BlockSource &,BlockPos const &,int)const 
	MCINLINE bool shouldConnectToRedstone(BlockSource & arg0,BlockPos const & arg1,int arg2) {
		bool (ChestBlock::*fnp)(BlockSource &,BlockPos const &,int)const;
		*((void **)&fnp) = dlsym("?shouldConnectToRedstone@ChestBlock@@UEBA_NAEAVBlockSource@@AEBVBlockPos@@H@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//void ChestBlock::updateSignalStrength(BlockSource &,BlockPos const &,int)const 
	MCINLINE void updateSignalStrength(BlockSource & arg0,BlockPos const & arg1,int arg2) {
		void (ChestBlock::*fnp)(BlockSource &,BlockPos const &,int)const;
		*((void **)&fnp) = dlsym("?updateSignalStrength@ChestBlock@@QEBAXAEAVBlockSource@@AEBVBlockPos@@H@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	//bool ChestBlock::use(Player &,BlockPos const &,unsigned char)const 
	MCINLINE bool use(Player & arg0,BlockPos const & arg1,unsigned char arg2) {
		bool (ChestBlock::*fnp)(Player &,BlockPos const &,unsigned char)const;
		*((void **)&fnp) = dlsym("?use@ChestBlock@@UEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z");
		return (this->*fnp)(arg0,arg1,arg2);
	}
	// ChestBlock::~ChestBlock()
	MCINLINE ~ChestBlock() {
		void(ChestBlock::*fnp)()const;
		*((void **)&fnp) = dlsym("??1ChestBlock@@UEAA@XZ");
		(this->*fnp)();
	}
};
