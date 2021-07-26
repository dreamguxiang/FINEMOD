// BDXLand.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "LLland.h"
#include "shared.h"
#include <map>
using namespace std;
void Version() {
	cout << "[LLland] version 210323" << endl;
}
static int LAND_MIN_Y = 0;
static bool LAND_EXPLODE;
std::time_t getTimeStamp()
{
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	return timestamp;
}

map<string, long > mc;
std::map < string, long > ::reverse_iterator iter;

static Logger LOG(stdio_commit{ "[LAND] " });
LangPack LangP("plugins/LiteLand/langpack/land.json");
#define to_lpos(x) ((x) ^ 0x80000000)
namespace LandImpl {
	using std::list;
	using std::swap;
	using std::unordered_map;
	static std::unique_ptr<KVDBImpl> db;
	typedef u64 xuid_t;

	//FastLand in shared.h
	static_assert(sizeof(FastLand) == sizeof(u32) * 8);
	static_assert(offsetof(FastLand, owner) == sizeof(u32) * 8);
	static_assert(offsetof(FastLand, perm_others) == sizeof(u32) * 6 + sizeof(u16));
	struct DataLand {
		lpos_t x, z, dx, dz; //4*int
		u32 lid; //5*int
		u16 spec;  //refcount
		LandPerm perm_group;//6*int
		u16 dim;
		LandPerm perm_others; //7*int
		string owner;
		DataLand() {}
		DataLand(FastLand const& fastland) {
			x = fastland.x;
			z = fastland.z;
			dx = fastland.dx;
			dz = fastland.dz;
			lid = fastland.lid;
			spec = fastland.refcount;
			dim = fastland.dim;
			perm_group = fastland.perm_group;
			perm_others = fastland.perm_others;
			owner = fastland.getOwnerView();
		}
		void addOwner(xuid_t x, bool SuperOwner = false) {
			if (SuperOwner) owner.insert(0, to_view(x)); else owner.append(to_view(x));
		}
		void delOwner(xuid_t x) {
			size_t pos = owner.find(to_view(x));
			if (pos != owner.npos)
				owner.erase(pos, sizeof(x));
		}
		string serialize() const {
			WBStream ws;
			ws.apply(x, z, dx, dz, lid, spec, perm_group, dim, perm_others, owner);
			return ws.data;
		}
		void deserialize(RBStream rs) {
			rs.apply(x, z, dx, dz, lid, spec, perm_group, dim, perm_others, owner);
		}
	};
	static inline int __fail(const string& c, int line) {
		printf("[LAND/FATAL :: %d] %s\n", line, c.c_str());
		exit(1);
		return 1;
	}
#define LAssert(x,y) (x)&&__fail(y,__LINE__)
	namespace LandCacheManager {
		static unordered_map<u32, FastLand*> cache;
		static void noticeFree(u32 id) {
			auto it = cache.find(id);
			LAssert(it == cache.end(), "double free land " + S(id));
			it->second->refcount--;
			if (it->second->refcount == 0) {
				free(it->second);
				cache.erase(it);
			}
		}
		static FastLand* requestLand(u32 id) {
			auto it = cache.find(id);
			if (it == cache.end()) {
				string landstr;
				LAssert(!db->get(to_view(id), landstr), "bad land " + S(id));
				FastLand* res = (FastLand*)malloc(landstr.size());
				if (!res) {
					LOG.l<LOGLVL::Fatal>("bad_alloc");
					exit(1);
				}
				memcpy(res, landstr.data(), landstr.size());
				LAssert(landstr.size() < sizeof(FastLand), "bad land " + S(id) + " " + S(landstr.size()));
				res->refcount = 1;
				cache[id] = res;
				return res;
			}
			else {
				it->second->refcount++;
				return it->second;
			}
		}
	};
	struct ChunkLandManager {
		FastLand* lands[16][16];
		u32 mLandsCnt;
		u32 managed_lands[256];
		~ChunkLandManager() {
			for (u32 i = 0; i < mLandsCnt; ++i) {
				LandCacheManager::noticeFree(managed_lands[i]);
			}
		}
		ChunkLandManager(u32* landlist, u32 siz, lpos_t xx, lpos_t zz) {
			memset(lands, 0, sizeof(lands));
			mLandsCnt = siz;
			memcpy(managed_lands, landlist, siz * sizeof(u32));
			LAssert(mLandsCnt >= 256, "mlandscnt " + S(mLandsCnt));
			for (u32 I = 0; I < siz; ++I) {
				auto fl = LandCacheManager::requestLand(landlist[I]);
				lpos_t sx, dx, sz, dz;
				if ((fl->x >> 4) == xx)
					sx = fl->x & 15;
				else
					sx = 0;
				if ((fl->z >> 4) == zz)
					sz = fl->z & 15;
				else
					sz = 0;
				if ((fl->dx >> 4) == xx)
					dx = fl->dx & 15;
				else
					dx = 15;
				if ((fl->dz >> 4) == zz)
					dz = fl->dz & 15;
				else
					dz = 15;
				for (lpos_t i = sx; i <= dx; ++i) {
					for (lpos_t j = sz; j <= dz; ++j) { lands[i][j] = fl; }
				}
			}
		}
	};

	static inline u64 ChunkID(lpos_t x, lpos_t z, u32 dim) {
		u32 ids[2];
		ids[0] = x;
		ids[1] = z;
		ids[1] |= dim << 29;
		return (*(u64*)(&ids));
		/*u64 rv = x;
		rv *= 0x100000000ull;
		rv += ((u64)z) * 16;
		rv += dim&3;
		return rv;*/
	}
	template<typename TP, size_t CAP = 24>
	struct pool_allocator {
		static_queue<TP*, CAP> q;
		inline TP* _get() {
			if (!q.empty()) {
				auto rv = q.back();
				q.pop_back();
				return rv;
			}
			return (TP*)malloc(sizeof(TP));
		}
		inline void _put(TP* p) {
			if (!q.full()) q.push_back(p);
			else free(p);
		}
		template<typename... T>
		TP* alloc(T&&... args) {
			TP* r = _get();
			new (r)TP(std::forward<T>(args)...);
			return r;
		}
		void dealloc(TP* t) {
			t->~TP();
			_put(t);
		}
	};
	static pool_allocator<ChunkLandManager> CLDP;
#if 1
	struct ChunkDataWrapped
	{
		ChunkLandManager* v;
		ChunkDataWrapped(ChunkDataWrapped const&) = delete;
		ChunkDataWrapped& operator=(ChunkDataWrapped const&) = delete;
		ChunkDataWrapped() {
			v = nullptr;
		}
		ChunkDataWrapped(lpos_t x, lpos_t z, int dim) {
			string LList;
			u64 CID = ChunkID(x, z, dim);
			if (db->get(to_view(CID), LList)) {
				v = CLDP.alloc((u32*)LList.data(), (u32)(LList.size() / sizeof(u32)), x, z);
				//v=new ChunkLandManager((u32*)LList.data(), (u32)(LList.size() / sizeof(u32)), x, z);
			}
			else {
				v = nullptr;
			}
		}
		~ChunkDataWrapped() {
			if (v) {
				//delete v;
				CLDP.dealloc(v);
				v = nullptr;
			}
		}
		FastLand const* get(int x, int z) {
			if (!v) return nullptr;
			return v->lands[x][z];
		}
	};
#else
	struct ChunkDataWrapped
	{
		ChunkLandManager v;
		ChunkDataWrapped() :v(EMPTYC) {
		}
		ChunkDataWrapped(lpos_t x, lpos_t z, int dim) :v(EMPTYC) {
			string LList;
			u64 CID = ChunkID(x, z, dim);
			if (db->get(to_view(CID), LList)) {
				//v = CLDP.alloc((u32*)LList.data(), (u32)(LList.size() / sizeof(u32)), x, z);
				//LOG(LList.size());
				v = ChunkLandManager((u32*)LList.data(), (u32)(LList.size() / sizeof(u32)), x, z);
			}
			else {
				v = EMPTYC;
			}
		}
	};
#endif
	static U64LRUmap<ChunkDataWrapped, 73> LRUM_CHUNK(256);
	static inline void purge_cache() {
		LRUM_CHUNK.clear();
		LAssert(LandCacheManager::cache.size(), "LandCacheManager::cache.size()");
	}
#if 1
	static inline ChunkDataWrapped& getChunkMan(lpos_t x, lpos_t z, int dim) {
		u64 CID = ChunkID(x, z, dim);
		auto res = LRUM_CHUNK.find(CID);
		if (res) {
			return *res;
		}
		else {
			return *LRUM_CHUNK.insert(CID, x, z, dim);
		}
	}
#endif
#if 0
	static inline ChunkLandManager* getChunkMan(lpos_t x, lpos_t z, int dim) {
		glb = ChunkDataWrapped(x, z, dim);
		return &glb.v;
	}
#endif
	static inline FastLand const* getFastLand(int x, int z, int dim) {
		ChunkDataWrapped& cm = getChunkMan(to_lpos(x) >> 4, to_lpos(z) >> 4, dim);
		return cm.get(x & 15, z & 15);
	}

	static u32 getLandUniqid() {
		string val;
		std::random_device rd;
		while (1) {
			u32 lid = rd();
			if (!lid) continue;
			if (!db->get(to_view(lid), val)) return lid;
		}
	}
	static void proc_chunk_add(lpos_t x, lpos_t dx, lpos_t z, lpos_t dz, int dim, u32 lid) {
		for (auto i = x; i <= dx; ++i) {
			for (auto j = z; j <= dz; ++j) {
				u64 key = ChunkID(i, j, dim);
				string val;
				db->get(to_view(key), val);
				val.append(to_view(lid));
				db->put(to_view(key), val);
			}
		}
	}
	static void proc_chunk_del(lpos_t x, lpos_t dx, lpos_t z, lpos_t dz, int dim, u32 lid) {
		for (auto i = x; i <= dx; ++i) {
			for (auto j = z; j <= dz; ++j) {
				u64 key = ChunkID(i, j, dim);
				string val;
				db->get(to_view(key), val);
				for (size_t i = 0; i < val.size(); i += sizeof(u32)) {
					if (*(u32*)(val.data() + i) == lid) {
						val.erase(i, 4);
						break;
					}
				}
				if (val.size())
					db->put(to_view(key), val);
				else
					db->del(to_view(key));
			}
		}
	}
	static void addLand(int _x, int _dx, int _z, int _dz, int dim, xuid_t owner, LandPerm perm = NOTHING) {
		lpos_t x(to_lpos(_x)), z(to_lpos(_z)), dx(to_lpos(_dx)), dz(to_lpos(_dz));
		DataLand ld;
		ld.x = x, ld.z = z, ld.dx = dx, ld.dz = dz, ld.dim = dim;
		ld.owner = to_view(owner);
		ld.perm_others = perm;
		ld.perm_group = PERM_FULL;
		auto lid = getLandUniqid();
		ld.lid = lid;
		db->put(to_view(lid), ld.serialize());
		proc_chunk_add(x >> 4, dx >> 4, z >> 4, dz >> 4, dim, lid);
		purge_cache();
	}
	static void updLand(DataLand const& ld) {
		db->put(to_view(ld.lid), ld.serialize());
		purge_cache();
	}
	static void removeLand(FastLand const* land) {
		db->del(to_view(land->lid));
		proc_chunk_del(land->x >> 4, land->dx >> 4, land->z >> 4, land->dz >> 4, land->dim, land->lid);
		purge_cache();
	}
	static inline void INITDB() {
		db = MakeKVDB(GetDataPath("land_v2"), true, 4, 10);
	}
};

static playerMap<PointSelector> SELECT_POINT;
int BUY_PRICE, SELL_PRICE;
static bool PROTECT_FARM, PROTECT_IFRAME;

#pragma region CMDENUM
enum class LANDOP :int {
	buy = 1,
	sell = 2,
	info = 3,
	gui = 4
};
enum class LANDOP2 :int {
	trust = 1,
	untrust = 2,
	give = 3
};
enum class LANDOP_PERM :int {
	perm_o = 1, //others
	perm_g = 2 //group
};
enum class LANDPOP :int {
	A = 1,
	B = 2,
	exit = 3,
	gui = 4
};
enum class LANDm :int {
	all = 1,
	query = 2
};
#pragma endregion 
static bool oncmd(CommandOrigin const& ori, CommandOutput& outp, MyEnum<LANDPOP> op) {
	auto wp = MakeSP(ori);
	if (!wp) return false;
	switch (op.val)
	{
	case LANDPOP::A: {
		SELECT_POINT[wp].mode = 1;
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 请右键选择点A");
	}
				   break;
	case LANDPOP::B: {
		SELECT_POINT[wp].mode = 2;
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 请右键选择点B");
	}
				   break;
	case LANDPOP::exit: {
		SELECT_POINT._map.erase(wp);
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 退出选点");
	}
					  break;
	case LANDPOP::gui: {
	}
					  break;
	default:
		break;
	}
	return true;
}

enum class LandFetchRes {
	success = 1,
	noland = 2,
	noperm = 3
};

using LandImpl::getFastLand, LandImpl::DataLand;
static std::pair<LandFetchRes, FastLand const*> FetchLandForOwner(WPlayer wp) {
	IVec2 pos(wp->getPos());
	auto fl = getFastLand(pos.x, pos.z, wp.getDimID());
	if (!fl) {
		return { LandFetchRes::noland,nullptr };
	}
	return { (fl->getOPerm(offPlayer::getXUID(wp)) == 2 || wp.getPermLvl() > 0) ? LandFetchRes::success : LandFetchRes::noperm,fl };
}
static FastLand const* genericPerm(int x, int z, WPlayer wp, LandPerm pm = LandPerm::NOTHING) {
	//WHY return FastLand*,for further land perm tips
	if (wp.getPermLvl() > 0) return nullptr;
	auto fl = getFastLand(x, z, wp.getDimID());
	if (!fl) return nullptr;
	if (fl->hasPerm(offPlayer::getXUID(wp), pm)) return nullptr;
	return fl;
}

LLLAND_API u32 getLandIDAt(IVec2 vc, int dim) {
	auto fl = getFastLand(vc.x, vc.z, dim);
	if (fl) return fl->lid; else return 0;
}
LLLAND_API u32 checkLandRange(IVec2 vc, IVec2 vc2, int dim) {
	for (int i = vc.x; i <= vc2.x; ++i)
		for (int j = vc.z; j <= vc2.z; ++j)
		{
			auto fl = getFastLand(i, j, dim);
			if (fl) return fl->lid;
		}
	return 0;
}
LLLAND_API bool checkLandOwnerRange(IVec2 vc, IVec2 vc2, int dim, unsigned long long xuid) {
	for (int i = vc.x; i <= vc2.x; ++i)
		for (int j = vc.z; j <= vc2.z; ++j)
		{
			auto fl = getFastLand(i, j, dim);
			if (fl && fl->getOPerm(xuid) == 0) return false;
		}
	return true;
}

static inline FastLand const* genericPerm(BlockPos const& pos, WPlayer wp, LandPerm pm = LandPerm::NOTHING) {
	if (pos.y < LAND_MIN_Y) return nullptr;
	return genericPerm(pos.x, pos.z, wp, pm);
}
static bool oncmd_2(CommandOrigin const& ori, CommandOutput& outp, MyEnum<LANDOP> op) {
	auto sp = MakeSP(ori);
	if (!sp) return false;
	auto wp = WPlayer(*sp);
	switch (op.val)
	{
	case LANDOP::buy: {
		auto it = SELECT_POINT._map.find(sp);
		if (it != SELECT_POINT._map.end()) {
			auto& point = it->second;
			if (!point.selected()) {
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.need.select"));
				return false;
			}
			auto [x, dx, z, dz] = point.pos();
			auto dim = point.dim;
			if (!land_buy_first_step(wp, outp, point)) return false;
			LOG("land buy step1", x, dx, z, dz);
			for (int i = x; i <= dx; ++i) {
				for (int j = z; j <= dz; ++j) {
					if (getFastLand(i, j, dim)) {
						outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.overlap"));
						outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r  X:"s + std::to_string(i) + " Z:" + std::to_string(j));
						return false;
					}
				}
			}
			if (!land_buy_next_step(wp, outp, point)) return false;
			LOG("land buy step2", x, dx, z, dz);
			LandImpl::addLand(x, dx, z, dz, dim, offPlayer::getXUID(sp));
			LOG("buy land", x, z, dx, dz, dim);
			SELECT_POINT._map.erase(sp);
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 购买成功！");
			return true;
		}
		else {
			return false;
		}
	}
					break;
	default: {
		auto [res, fl] = FetchLandForOwner(wp);
		if (res == LandFetchRes::noland) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.land.here"));
			return false;
		}
		if (op == LANDOP::info) {
			outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 领地信息");
			outp.addMessage(u8"领地id： " + S(fl->lid));
			outp.addMessage(u8"拥有者： " + fl->ownStr());
			outp.addMessage(u8"信任者权限： " + S((u16)fl->perm_group)); //TODO:human readable perm
			outp.addMessage(u8"游客权限： " + S((u16)fl->perm_others));
			return true;
		}
		else {
			if (res == LandFetchRes::noperm) {
				outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.perm"));
				return false;
			}
			//sell or gui
			if (op == LANDOP::sell) {
				land_sell_helper(wp, outp, fl);
				LandImpl::removeLand(fl);
				return true;
			}
		}
	}
		   break;
	}
	return true;
}
static bool oncmd_3(CommandOrigin const& ori, CommandOutput& outp, MyEnum<LANDOP2> op, string& target) {
	auto sp = MakeSP(ori);
	if (!sp) return false;
	WPlayer wp{ *sp };
	auto [res, fl] = FetchLandForOwner(wp);
	if (res == LandFetchRes::noland) {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.land.here"));
		return false;
	}
	if (res == LandFetchRes::noperm) {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.perm"));
		return false;
	}
	DataLand dl(*fl);
	auto tar = XIDREG::str2id(target);
	if (!tar.set) {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.target"));
		return false;
	}
	switch (op.val)
	{
	case LANDOP2::trust: {
		dl.addOwner(tar.val());
	}
					   break;
	case LANDOP2::untrust: {
		if (dl.owner.size() == sizeof(xuid_t)) {
			return false;
		}
		dl.delOwner(tar.val());
	}
						 break;
	case LANDOP2::give: {
		if (dl.owner.size() == 0) {
			return false;
		}
		memcpy(dl.owner.data(), &tar.val(), sizeof(xuid_t));
	}
					  break;
	};
	updLand(dl);
	return true;
}
static bool oncmd_perm(CommandOrigin const& ori, CommandOutput& outp, MyEnum<LANDOP_PERM> op, int perm) {
	auto wp = MakeWP(ori);
	auto [res, fl] = FetchLandForOwner(wp.val());
	if (res == LandFetchRes::noland) {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.land.here"));
		return false;
	}
	if (res == LandFetchRes::noperm) {
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.no.perm"));
		return false;
	}
	DataLand dl(*fl);
	if (op.val == LANDOP_PERM::perm_o) {
		dl.perm_others = (decltype(dl.perm_others))perm;
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 执行成功");
	}
	else {
		dl.perm_group = (decltype(dl.perm_others))perm;
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 执行成功");
	}
	updLand(dl);
	return true;
}
FastLand const* getFastLand_export(int x, int z, int dim) {
	return LandImpl::getFastLand(x, z, dim);
}
static inline void NoticePerm(WPlayer wp, FastLand const*,string a1) {
	wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r "+ _TRS("land.no.perm")+ u8"缺少权限 "+a1, RAW);
}
static void loadConfig() {
	try {
		ConfigJReader jr("plugins/LiteLand/land.json");
		jr.bind("LAND_EXPLODE", LAND_EXPLODE,true);
		jr.bind("BUY_PRICE", BUY_PRICE);
		jr.bind("SELL_PRICE", SELL_PRICE);
		jr.bind("PROTECT_FARM", PROTECT_FARM);
		jr.bind("PROTECT_IFRAME", PROTECT_IFRAME);
		jr.bind("LAND_MIN_Y", LAND_MIN_Y);
	}
	catch (string e) {
		LOG.p<LOGLVL::Fatal>("json error", e);
		throw 0;
	}
}

static void TEST() {
	using namespace LandImpl;
	std::random_device rdv;
	srand((unsigned)time(NULL) * 1000);
	int X = 10;
	int all = 0;
	int z = 0;
	timespec tt;
	//int all1 = 0;
	//int all2 = 0;
	auto ab = rdv();
	auto bb = rdv();
	long long a2 = getTimeStamp();
	for (int i = 0; i <= 1000; ++i)
		for (int j = 0; j <= 1000; ++j) {
			z = z + 0.1;
			LandImpl::addLand((int)(ab+i+j)*114514, (int)(ab + i + j) * 114514 +z, (int)(bb+i+j) * 114514, (int)(bb + i + j) * 114514 +z, ((u32)i * 233 + (u32)j) & 3, i * 1000 + j);
		}
	long long b2 = getTimeStamp();
	auto a = rdv();
	auto b = rdv();
	auto c = rdv();
	long long a1 = getTimeStamp();
	while (X-- > 0)
		for (int i = 0; i <= 1000000; ++i, i % 10 == 0 ) {
			//for (int j = -1000; j <= 1000; ++j) {
			//	LandImpl::getFastLand(i, j, 0);
			//	LandImpl::getFastLand(i, j, 1);
			//	LandImpl::getFastLand(i, j, 2);
			LandImpl::getFastLand((int)a+i, (int)b+i, c & 3);
			//}
			//if ((rdv() & 3) == 0) LandImpl::purge_cache();
			//DBG_FLAG = 1;
		}
	long long b1 = getTimeStamp();
	/*
	cout << all << endl;
	printf("enter test\n");
	for (int i = -1000; i <= 100; ++i)
		for (int j = -1000; j <= 100; ++j) {
			LandImpl::addLand(i, i, j, j, ((u32)i * 233 + (u32)j) & 3, i * 1000 + j);
		}
	printf("->1\n");
	long long a3 = getTimeStamp();
	for (int i = -1000; i <= 1000; ++i)
		for (int j = -1000; j <= 1000; ++j) {
			auto fl = LandImpl::getFastLand(i, j, ((u32)i * 233 + (u32)j) & 3);
			//all2 = all2 + 1;
			LAssert((!fl && i >= -1000 && i <= 100 && j >= -1000 && j <= 100) || (fl && fl->owner[0] != i * 1000 + j), "test failed");
			fl = LandImpl::getFastLand(i, j, ((u32)i * 233 + (u32)j + 1) & 3);
			LAssert(fl, "test2 failed");
		}
	long long b3 = getTimeStamp();
	printf("->2\n");
	X = 10;
	long long a2 = getTimeStamp();
	while (X-- > 0)
		for (int i = 0; i <= 1000; ++i, i % 10 == 0) {
			//for (int j = -1000; j <= 1000; ++j) {
			//	LandImpl::getFastLand(i, j, 0);
			//	LandImpl::getFastLand(i, j, 1);
			//	LandImpl::getFastLand(i, j, 2);
			for (int Z = 0; Z < 1000; ++Z) {
				LandImpl::getFastLand((int)rdv(), (int)rdv(), rdv() & 3);
				//all2 = all2 + 1;
			}
			//}
			//if ((rdv() & 3) == 0) LandImpl::purge_cache();
			//DBG_FLAG = 1;
		}
	long long b2 = getTimeStamp();
	*/	
	cout << "--------------------------------" << endl;	
	cout << "create 100w lands" << endl;
	cout << "time:" << to_string((float)(b2 - a2) / 1000000000) << " s" << endl;
	cout << "\n--------------------------------"<<endl;
	cout << "100w random query 100w lands" << endl;
	cout << "time:" << to_string((float)(b1 - a1)/1000000000) << " s" << endl;
	cout << "--------------------------------" << endl;
	//cout << "1000w directional query 100w lands" << endl;
	//cout << "time:" << b3 - a3 << "ms" << endl;
	//cout << "--------------------------------" << endl;
	//cout << "1000w random query 100w lands" << endl;
	//cout << "time:" << b2 - a2 << "ms" << endl;
	//cout << "--------------------------------" << endl;
	printf("TEST DONE\n");
}


static void FIX_BUG_0401() {
	using LandImpl::db;
	string val;
	if (!db->get("BUG_0402_FIXED", val)) {
		LOG("\n\nstart to fix BUG_0402\n\n");
		vector<std::tuple<u32, u32, u32, u32, int, u32>> lands;
		vector<string> to_delete;
		db->iter([&](string_view k, string_view v) {
			if (k.size() == 4) {
				FastLand* fl = (FastLand*)v.data();
				if (fl->dx >= fl->x && fl->dz >= fl->z && v.size() >= sizeof(FastLand)) {
					lands.emplace_back(fl->x, fl->dx, fl->z, fl->dz, fl->dim, fl->lid);
				}
				else {
					LOG.p<LOGLVL::Error>("found buggy land", fl->lid, fl->x, fl->dx, fl->z, fl->dz);
					to_delete.emplace_back(k);
				}
			}
			else {
				if (k != "FIX_LAND_PERM_GROUP_0405")
					to_delete.emplace_back(k);
			}
			return true;
			});
		for (auto& i : to_delete) {
			db->del(i);
		}
		for (auto& [x, dx, z, dz, dim, lid] : lands)
			LandImpl::proc_chunk_add(x >> 4, dx >> 4, z >> 4, dz >> 4, dim, lid);
		db->put("BUG_0402_FIXED", "fixed");
	}
}
void CHECK_MEMORY_LAYOUT() {
	DataLand ld;
	xuid_t xid = 114514;
	ld.owner.append(to_view(xid));
	auto sz = ld.serialize().size();
	using LandImpl::__fail;
	LAssert(sz != sizeof(FastLand) + sizeof(xuid_t), "MEM_LAYOUT_ERR");
}
void FIX_LAND_PERM_GROUP_0405() {
	using LandImpl::db;
	string val;
	if (!db->get("FIX_LAND_PERM_GROUP_0405", val)) {
		LOG("\n\nstart to fix FIX_LAND_PERM_GROUP_0405\n\n");
		std::vector<pair<u32, string>> lands;
		db->iter([&](string_view k, string_view v) {
			if (k.size() == 4) {
				string vv{ v };
				FastLand* fl = (FastLand*)vv.data();
				static_assert(offsetof(FastLand, perm_group) <= 36);
				fl->perm_group = PERM_FULL;
				lands.emplace_back(pair{ *(u32*)k.data(), std::move(vv) });
			}
			return true;
			});
		for (auto& [k, v] : lands) {
			db->put(to_view(k), v);
		}
		db->put("FIX_LAND_PERM_GROUP_0405", "fixed");
	}
}
bool oncmd_landop(CommandOrigin const& ori, CommandOutput& outp, MyEnum<LANDm> op, optional<string>& target) {
	switch (op)
	{
	case LANDm::query:
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 玩家 "+ target.val() +u8" 领地信息");
		LandImpl::db->iter([&](string_view k, string_view v) {
			if (k.size() == 4) {
				FastLand* fl = (FastLand*)v.data();
				transform(target.val().begin(), target.val().end(), target.val().begin(),::tolower);
				if (target.set && target.val().c_str() == fl->ownStr()) {
					if (k.size() == 4) {
						outp.addMessage(u8"l领地id： " + S(fl->lid) + u8" 权限：" + S(fl->perm_group) + " : " + S(fl->perm_others));
						outp.addMessage(u8"坐标： (" + S(int(to_lpos(fl->x))) + " " + S(int(to_lpos(fl->z))) + ") -> (" + S(int(to_lpos(fl->dx))) + " " + S(int(to_lpos(fl->dz))) + u8") 维度： " + S(fl->dim));
						outp.addMessage(u8"拥有者： " + fl->ownStr());
						outp.addMessage("----------------------");
					}
				}
			}
			return true;
			});
		break;
	case LANDm::all:
		outp.addMessage(u8"§l§6[§eMCBE §bFINE§6]§r 全服领地信息");
		LandImpl::db->iter([&](string_view k, string_view v) {
			if (k.size() == 4) {
				FastLand* fl = (FastLand*)v.data();
				outp.addMessage(u8"领地id： " + S(fl->lid) + u8" 权限：" + S(fl->perm_group) + " : " + S(fl->perm_others));
				outp.addMessage(u8"坐标： (" + S(int(to_lpos(fl->x))) + " " + S(int(to_lpos(fl->z))) + ") -> (" + S(int(to_lpos(fl->dx))) + " " + S(int(to_lpos(fl->dz))) + u8") 维度： " + S(fl->dim));
				outp.addMessage(u8"拥有者： " + fl->ownStr());
				outp.addMessage("----------------------");
			}
			return true;
			});
		break;
	}
	return true;
}

void InitLUA();

void entry() {
	Version();
	LandImpl::INITDB();
	CHECK_MEMORY_LAYOUT();
	FIX_BUG_0401();
	FIX_LAND_PERM_GROUP_0405();
	//WaitForDebugger();
	//TEST();
	InitLUA();
	loadConfig();
	Event::addEventListener([](RegCmdEV ev) {
		CMDREG::SetCommandRegistry(ev.CMDRg);
		CEnum<LANDPOP> _1("landpoint", { "a","b","exit","gui" });
		CEnum<LANDOP> _2("landop", { "buy","sell","info" });
		CEnum<LANDOP2> _3("landop2", { "trust","untrust","give" });
		CEnum<LANDOP_PERM> _4("landperm", { "permo","permg" });
		CEnum<LANDm> _5("landop3", { "all","query" });
		MakeCommand("land", "land command", 0);
		CmdOverload(land, oncmd, "point-op");
		CmdOverload(land, oncmd_2, "land-op");
		CmdOverload(land, oncmd_3, "land-op2", "target");
		CmdOverload(land, oncmd_perm, "perm", "land_perm");
		MakeCommand("land_op", "land op", 1);
		CmdOverload(land_op, oncmd_landop,"op","name");
		});

}
THook(void*, "?attack@ItemFrameBlock@@UEBA_NPEAVPlayer@@AEBVBlockPos@@@Z", void* a, void* b, void* c) {
	return nullptr;
}
THook(void*, "?transformOnFall@FarmBlock@@UEBAXAEAVBlockSource@@AEBVBlockPos@@PEAVActor@@M@Z", void* t, class BlockSource& x, class BlockPos const& y, class Actor* z, float p) {
	return nullptr;
}
THook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVItemFrameDropItemPacket@@@Z", ServerNetworkHandler* thi, NetworkIdentifier& a2, unsigned char* pk) {
	auto sp = thi->_getServerPlayer(a2, pk[16]);
	if (sp) {
		int x = dAccess<int, 48>(pk);
		int z = dAccess<int, 56>(pk);
		WPlayer wp{ *sp };
		auto fl = genericPerm(x, z, wp, LandPerm::PERM_ITEMFRAME);
		if (fl) {
			NoticePerm(wp, fl,"itemframe");
			return;
		}
	}
	return original(thi, a2, pk);
}

THook(bool, "?mayPlace@BlockSource@@QEAA_NAEBVBlock@@AEBVBlockPos@@EPEAVActor@@_N@Z",
	BlockSource* bs, Block* blk, BlockPos* blkpos, unsigned char a4, Actor* ac, bool a5) {
	auto pl = MakeSP(ac);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(blkpos->x, blkpos->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return false;
		}	
	}
	return original(bs, blk, blkpos, a4, ac, a5);
}

THook(bool, "?checkBlockDestroyPermissions@BlockSource@@QEAA_NAEAVActor@@AEBVBlockPos@@AEBVItemStackBase@@_N@Z",
	BlockSource* _this, struct Actor* a2, const struct BlockPos* pos, const struct ItemStackBase* a4, bool a5) {
	auto wp = WPlayer(*(ServerPlayer*)a2);
	auto fl = genericPerm(pos->x, pos->z, wp, LandPerm::PERM_DESTROY);
	if (fl) {
		NoticePerm(wp, fl,"destroy");
		return false;
	}
	return original(_this, a2, pos,a4,a5);
}

THook(__int64, "?_useOn@EndCrystalItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, __int64 a1, const struct ItemInstance* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}
THook(__int64, "?_useOn@LeadItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, __int64 a1, __int64 a2, Actor* a3, BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@LodestoneCompassItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, __int64 a1, __int64 a2, Actor* a3, BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}


THook(__int64, "?_useOn@RedStoneDustItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, __int64 a1, __int64 a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@ActorPlacerItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, void* a1, ItemStack* a2, Actor* a3, BlockPos* a4, unsigned __int8 a5) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4, a5);
}

THook(__int64, "?_useOn@ArmorStandItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, void* a1, ItemStack* a2, Actor* a3, BlockPos* a4, unsigned __int8 a5) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl, "build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4, a5);
}

THook(__int64, "?_useOn@BalloonItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, __int64 a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@BambooBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4, unsigned __int8 a5) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4, a5);
}


THook(__int64, "?_useOn@BannerItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@ScaffoldingBlockItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@SeaPickleBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@ShearsItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@ShovelItem@@MEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@SignItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@SkullItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@TopSnowBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@WaterLilyBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@BedItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@BellBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@BoatItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@BottleItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}


THook(__int64, "?_useOn@BucketItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@CompassItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@CoralFanBlockItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@ComponentItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@DoorItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@DyePowderItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@FertilizerItem@@MEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@FireChargeItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@FireworksItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}


THook(__int64, "?_useOn@FlintAndSteelItem@@MEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@HangingActorItem@@EEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@DeprecatedSlabBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@SlabBlockItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(__int64, "?_useOn@CopperSlabItem@@UEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EMMM@Z"
	, struct Block* a1, ItemStackBase* a2, Actor* a3, const struct BlockPos* a4) {
	auto pl = MakeSP(a3);
	if (pl) {
		auto wp = WPlayer(*(ServerPlayer*)pl);
		auto fl = genericPerm(a4->x, a4->z, wp, LandPerm::PERM_BUILD);
		if (fl) {
			NoticePerm(wp, fl,"build");
			return 0;
		}
	}
	return original(a1, a2, a3, a4);
}

THook(bool, "?use@Block@@QEBA_NAEAVPlayer@@AEBVBlockPos@@E@Z",
	Block* a1, Player* a2, BlockPos* a3, unsigned __int8 a4) {
	auto wp = WPlayer(*(ServerPlayer*)a2);
	auto fl = genericPerm(a3->x, a3->z, wp, LandPerm::PERM_USE);
	if (fl) {
		NoticePerm(wp, fl, "use");
		return false;
	}
	return original(a1, a2, a3, a4);
}

THook(bool,
	"?useItemOn@GameMode@@UEAA_NAEAVItemStack@@AEBVBlockPos@@EAEBVVec3@@PEBVBlock@@@Z",
	void* thi, ItemStack* a2, BlockPos a3, unsigned __int8 a4, void* v5, Block* pBlk) {
	auto sp = *reinterpret_cast<Player**>(reinterpret_cast<unsigned long long>(thi) + 8);
	long a = getTimeStamp();
	auto playername = offPlayer::getRealName(sp);
	for (iter = mc.rbegin(); iter != mc.rend(); iter++)
		if (iter->first == playername) {
			if (a - mc[playername] <= (long)50) {
				return false;
			}
		}
	mc[playername] = a;
	mc.insert(std::map < string, long > ::value_type(playername, a));
	auto wp = WPlayer(*(ServerPlayer*)sp);
	LandPerm pm = sp->getCarriedItem().isNull() ? LandPerm::PERM_USE : LandPerm::PERM_USE;
	auto fl = genericPerm(a3.x, a3.z, wp, pm);
	if (fl) {
		NoticePerm(wp, fl,"use");
		return false;
	}
	auto fls = genericPerm(a3.x, a3.z ,wp, LandPerm::PERM_BUILD);
	string mstr;
	if (fls) {
		if (!a2->isNull()) {
			SymCall("?getSerializedName@Item@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", void, Item const*, string*)(a2->getItem(), &mstr);
			if (mstr == "fiercecraft:stone_smasher" || mstr == "fiercecraft:basic_wand") {
				NoticePerm(wp, fl,"build");
				return false;
			}
		}
	}
	auto it = SELECT_POINT._map.find(wp);
	if (it == SELECT_POINT._map.end()) return original(thi, a2, a3, a4, v5, pBlk);
	if (it->second.click(a3,sp->getDimensionId())) {
		if (it->second.mode == 1) {
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.select.A"), RAW);
		}
		else {
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.select.B"), RAW);
			wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r 大小： " + S(it->second.size()) + u8" 猫粮： " + S(it->second.size() * BUY_PRICE), RAW);
		}
	}
	else {
		wp.sendText(u8"§l§6[§eMCBE §bFINE§6]§r " + _TRS("land.select.err"), RAW);
	}
	return original(thi, a2, a3, a4, v5, pBlk);
}

THook(bool, "?_hurt@Mob@@MEAA_NAEBVActorDamageSource@@H_N1@Z", Mob*_this, ActorDamageSource* ads, int damage, bool unk1_1, bool unk2_0) {
	ActorUniqueID src_id = ads->getEntityUniqueID();
	IVec2 pos(_this->getPos());
	if (_this->getPos().y < LAND_MIN_Y) return original(_this, ads, damage, unk1_1, unk2_0);
	FastLand const* fl = LandImpl::getFastLand(pos.x, pos.z, _this->getDimensionId());
	if (fl) {
		if (ads->isEntitySource()) {
			auto level = offPlayer::getLevel(_this);
			char v83;
			auto v6 = *(void**)(*(__int64(__fastcall**)(void*, char*))(*(uintptr_t*)ads + 64))(ads, &v83);
			auto src = SymCall("?fetchEntity@Level@@UEBAPEAVActor@@UActorUniqueID@@_N@Z"
				, Actor*, Level*, void*, bool)(level, v6, false);
			auto sp = MakeSP(src);
			if (!sp) return original(_this, ads, damage, unk1_1, unk2_0);
			WPlayer wp{ *sp };
			if (genericPerm(pos.x, pos.z, wp, LandPerm::PERM_ATK)) {
				NoticePerm(wp, fl,"atk");
				return false;
			}
		}
	}
	return original(_this, ads, damage, unk1_1, unk2_0);
}


THook(void, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z", 
	Level* _this,  BlockSource* a2,  Actor* a3,  Vec3* a4, float a5, bool a6, bool a7, float a8, bool a9) {
	if (LAND_EXPLODE) {
		auto px = (int)a4->x;
		auto py = (int)a4->y;
		auto pz = (int)a4->z;
		if (px < 0)px = px - 1;
		if (pz < 0)pz = pz - 1;
		IVec2 pos{ px,pz };
		if (a4->y < LAND_MIN_Y) return;
		for (int i = -4; i < 5; ++i) {
			for (int j = -4; j < 5; ++j) {
				FastLand const* fl = LandImpl::getFastLand(pos.x + i, pos.z + j, a3->getDimensionId());
				if (fl) return;
			}
		}
	}
	return original(_this,a2,a3,a4,a5,a6,a7,a8,a9);
}

THook(void, "?explode@RespawnAnchorBlock@@CAXAEAVPlayer@@AEBVBlockPos@@AEAVBlockSource@@AEAVLevel@@@Z",
	Player* a1, BlockPos* a2, BlockSource* a3, Level* a4) {
	if (LAND_EXPLODE) {
		IVec2 pos{ a2->x,a2->z };
		if (a2->y < LAND_MIN_Y) return ;
		for (int i = -6; i < 7; ++i) {
			for (int j = -6; j < 7; ++j) {
				FastLand const* fl = LandImpl::getFastLand(pos.x + i, pos.z + j, a1->getDimensionId());
				if (fl) return;
			}
		}
	}
	return original(a1, a2, a3, a4);
}
THook(int, "?handle@ItemUseOnActorInventoryTransaction@@UEBA?AW4InventoryTransactionError@@AEAVPlayer@@_N@Z", void* thi, ServerPlayer* sp, bool unk) {
	auto rtid = dAccess<ActorRuntimeID, 104>(thi);
	auto id = dAccess<int, 112>(thi);
	if (!sp) return original(thi, sp, unk);
	IVec2 vc(sp->getPos());
	if (sp->getPos().y < LAND_MIN_Y) return original(thi, sp, unk);
	LandPerm pm = id == 1 ? LandPerm::PERM_ATK : LandPerm::PERM_INTERWITHACTOR;
	auto fl = genericPerm(vc.x, vc.z, WPlayer(*(ServerPlayer*)sp), pm);
	if (fl) {
		NoticePerm(WPlayer(*(ServerPlayer*)sp), fl,"atk or interwithactor");
		return 0;
	}
	return original(thi, sp, unk);
}
