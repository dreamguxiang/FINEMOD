#pragma once
#include"pch.h"
extern void InitLUA();
struct PointSelector {
	int mode;
	int dim;
	bool selectedA;
	bool selectedB;
	BlockPos A;
	BlockPos B;
	std::tuple<int, int, int, int> pos() {
		return { std::min(A.x,B.x),std::max(A.x,B.x),std::min(A.z,B.z),std::max(A.z,B.z) };
	}
	bool click(BlockPos pos, int _dim) {
		if (mode == 1) { //A
			selectedA = true;
			A = pos;
			dim = _dim;
			return true;
		}
		else {
			if (dim != _dim || !selectedA) return false;
			B = pos;
			selectedB = true;
			return true;
		}
	}
	size_t size() {
		auto [x, dx, z, dz] = pos();
		return ((size_t)dx - x + 1) * ((size_t)dz - z + 1);
	}
	bool selected() {
		return selectedA && selectedB;
	}
};
extern LangPack LangP;
extern int BUY_PRICE, SELL_PRICE;
#define OERR(x) outp.error(_TRS(x))
struct FastLand;
bool land_buy_first_step(WPlayer wp, CommandOutput& outp, PointSelector& point);
bool land_buy_next_step(WPlayer wp, CommandOutput& outp, PointSelector& point);
void land_sell_helper(WPlayer wp, CommandOutput& outp, FastLand const* fl);
typedef u32 lpos_t;
enum LandPerm : u16 {
	NOTHING = 0,   //no perm specified , must be owner to operate
	PERM_INTERWITHACTOR = 1,
	PERM_USE = 2,  //null hand place
	PERM_ATK = 4,  //attack entity
	PERM_BUILD = 8, //nonnull hand place
	PERM_DESTROY = 16,
	PERM_ITEMFRAME = 32,
	PERM_FULL = 63
};
struct FastLand {
	lpos_t x, z, dx, dz; //4*int
	u32 lid; //5*int
	u16 refcount;
	LandPerm perm_group; //6*int
	u16 dim;
	LandPerm perm_others; //7*int
	u32 owner_sz; //8*int
	xuid_t owner[0];
	inline int getOPerm(xuid_t x) const {
		u32 owner_sz2 = owner_sz >> 3;
		for (u32 i = 0; i < owner_sz2; ++i) {
			if (owner[i] == x) return 1 + (i == 0);
		}
		return 0;
	}
	inline bool hasPerm(xuid_t x, LandPerm PERM) const {
		switch (getOPerm(x))
		{
		case 0://others
			return (PERM & perm_others) != 0;
		case 1://group
			return (PERM & perm_group) != 0;
		case 2://owner
			return 1;
		}
		return 0; //never runs here
	}
	inline string_view getOwnerView() const {
		return { (char*)owner,owner_sz };
	}
	inline array_view<xuid_t> OWNER() const {
		return { owner,owner_sz >> 3 };
	}
	inline string ownStr() const {
		string rv;
		u32 owner_sz2 = owner_sz >> 3;
		for (u32 i = 0; i < owner_sz2; ++i) {
			auto name = XIDREG::id2str(owner[i]);
			if (name.set) {
				rv += name.val();
				rv += ',';
			}
			else {
				rv += S(owner[i]) + ',';
			}
		}
		if (rv.size()) rv.pop_back();
		return rv;
	}
};