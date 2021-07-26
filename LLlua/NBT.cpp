#include "pch.h"
#include"luaBindings.h"
#include <map>
using namespace std;
enum TagType {
	End, Byte, Short, Int, Int64, Float, Double, ByteArray, String, List, Compound,
};

constexpr unsigned int H(string_view s)
{
	auto str = s.data();
	unsigned int hash = 5381;
	while (*str) {
		hash = ((hash << 5) + hash) + (*str++); /* times 33 */
	}
	hash &= ~(1 << 31); /* strip the highest bit */
	return hash;
}

struct Tag{
private:
	char filler[40];
	template<typename T>
	T getVaule() {
		return *(T*)((uintptr_t)this + 8);
	}

	void setNode(lua_State* L, const char* type, const char* v) {
		lua_newtable(L);
		lua_pushstring(L, "type");
		lua_pushstring(L, type);
		lua_settable(L, -3);
		lua_pushstring(L, "vaule");
		lua_pushstring(L, v);
		lua_settable(L, -3);
	}

	void setNode(lua_State* L, const char* type, lua_Number num) {
		lua_newtable(L);
		lua_pushstring(L, "type");
		lua_pushstring(L, type);
		lua_settable(L, -3);
		lua_pushstring(L, "vaule");
		lua_pushnumber(L, num);
		lua_settable(L, -3);
	}
	
	void put(string k, Tag* v) {
		return SymCall("?put@CompoundTag@@QEAAAEAVTag@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$$QEAV2@@Z"
			, void, void*, string, Tag*)(this, k, v);
	}

	void put(string k, short v) {
		return SymCall("?putShort@CompoundTag@@QEAAAEAFV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@F@Z"
			, void, void*, string, short)(this, k, v);
	}

	void put(string k, string v) {
		return SymCall("?putString@CompoundTag@@QEAAAEAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V23@0@Z"
			, void, void*, string, string)(this, k, v);
	}

	void put(string k, int v) {
		return SymCall("?putInt@CompoundTag@@QEAAAEAHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@H@Z"
			, void, void*, string, int)(this, k, v);
	}

	void put(string k, __int64 v) {
		return SymCall("?putInt64@CompoundTag@@QEAAAEA_JV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_J@Z",
			void, void*, string, __int64)(this, k, v);
	}

	void put(string k, float v) {
		return SymCall("?putFloat@CompoundTag@@QEAAAEAMV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@M@Z"
			, void, void*, string, float)(this, k, v);
	}

	void put(string k, Tag** v) {
		return SymCall("?putCompound@CompoundTag@@QEAAPEAV1@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@3@@Z",
			void, void*, string, Tag**)(this, k, v);
	}

	void put(string k, char v) {
		return SymCall("?putByte@CompoundTag@@QEAAAEAEV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@E@Z"
			, void, void*, string, char)(this, k, v);
	}

	void addVaule(Tag* t) {
		SymCall("?add@ListTag@@QEAAXV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@@Z",
			void, void*, Tag**)(this, &t);
	}
	
	template<typename T>
	void setVaule(T v) {
		*(T*)((uintptr_t)this + 8) = v;
	}
	void tag2List(lua_State* L, Tag* tag) {
		vector<Tag*> taglist = tag->getVaule<vector<Tag*>>();
		lua_newtable(L);
		{
			lua_pushstring(L, "type");
			lua_pushstring(L, "List");
			lua_settable(L, -3);
			lua_pushstring(L, "vaule");
			lua_newtable(L);
			{
				int idx = 0;
				for (auto i : taglist) {
					switch (i->getTagType()) {
					case TagType::Byte:
						lua_pushinteger(L, ++idx);
						setNode(L, "Byte", i->getVaule<char>());
						lua_settable(L, -3);
						break;
					case TagType::Short:
						lua_pushinteger(L, ++idx);
						setNode(L, "Short", i->getVaule<short>());
						lua_settable(L, -3);
						break;
					case TagType::Int:
						lua_pushinteger(L, ++idx);
						setNode(L, "Int", i->getVaule<int>());
						lua_settable(L, -3);
						break;
					case TagType::Int64:
						lua_pushinteger(L, ++idx);
						setNode(L, "Int64", i->getVaule<__int64>());
						lua_settable(L, -3);
						break;
					case TagType::Float:
						lua_pushinteger(L, ++idx);
						setNode(L, "Float", i->getVaule<float>());
						lua_settable(L, -3);
						break;
					case TagType::Double:
						lua_pushinteger(L, ++idx);
						setNode(L, "Double", i->getVaule<double>());
						lua_settable(L, -3);
						break;
					case TagType::String:
						lua_pushinteger(L, ++idx);
						setNode(L, "String", i->getVaule<string>().c_str());
						lua_settable(L, -3);
						break;
					case TagType::ByteArray:
						break;
					case TagType::List:
						lua_pushinteger(L, ++idx);
						tag2List(L, i);
						lua_settable(L, -3);
						break;
					case TagType::Compound:
						lua_pushinteger(L, ++idx);
						i->deserializeToLuaTable(L);
						lua_settable(L, -3);
						break;
					}
				}
			}
			lua_settable(L, -3);
		}
	}

	static Tag* list2Tag(luabridge::LuaRef list) {
		auto tag = Tag::createTag(TagType::List);
		auto nbt = luabridge::Iterator::Iterator(list, false);
		auto v = nbt.value();
		while (!nbt.isNil()) {
			switch (H(v["type"].tostring()))
			{
			case H("Byte"):
			{
				auto tmp = Tag::createTag(TagType::Byte);
				tmp->setVaule(v["vaule"].cast<char>());
				tag->addVaule(tmp); 
			}
				break;
			case H("Short"):
			{
				auto tmp = Tag::createTag(TagType::Short);
				tmp->setVaule(v["vaule"].cast<short>());
				tag->addVaule(tmp);
			}
				break;
			case H("Int"):
			{
				auto tmp = Tag::createTag(TagType::Int);
				tmp->setVaule(v["vaule"].cast<int>());
				tag->addVaule(tmp);
			}
				break;
			case H("Int64"):
			{
				Tag* tmp = Tag::createTag(TagType::Int64);
				tmp->setVaule(v["vaule"].cast<__int64>());
				tag->addVaule(tmp); 
			}
				break;
			case H("Float"):
			{
				auto tmp = Tag::createTag(TagType::Float);
				tmp->setVaule(v["vaule"].cast<float>());
				tag->addVaule(tmp); 
			}
				break;
			case H("String"):
			{
				auto tmp = Tag::createTag(TagType::String);
				tmp->setVaule(v["vaule"].cast<string>());
				tag->addVaule(tmp);
			}
				break;
			case H("List"):
			{
				auto t = list2Tag(v["vaule"]);
				tag->addVaule(t); 
			}
				break;
			case H("Compound"):
			{
				auto t = toTag(v);
				tag->addVaule(t);
			}
				break;
			}
			nbt.next();
		}
		return tag;
	}
public:
	static Tag* createTag(TagType t) {
		Tag* tag = 0;
		SymCall("?newTag@Tag@@SA?AV?$unique_ptr@VTag@@U?$default_delete@VTag@@@std@@@std@@W4Type@1@@Z",
			void, Tag**, TagType)(&tag, t);
		return tag;
	}
	char getTagType() {
		return (*(__int64(__fastcall**)(const struct Tag*))(*(uintptr_t*)this + 40i64))(this);
	}

	void deserializeToLuaTable(lua_State* L) {
		lua_newtable(L);
		{
			lua_pushstring(L, "type");
			lua_pushstring(L, "Compound");
			lua_settable(L, -3);
			lua_pushstring(L, "vaule");
			lua_newtable(L);
			{
				auto top = lua_gettop(L);
				auto tag = *(map<string, Tag>*)((uintptr_t)this + 8);
				for (auto i : tag) {
					switch (i.second.getTagType()) {
					case TagType::End:
						break;
					case TagType::Byte:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Byte", i.second.getVaule<char>());
						lua_settable(L, -3);
						break;
					case TagType::Short:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Short", i.second.getVaule<short>());
						lua_settable(L, -3);
						break;
					case TagType::Int:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Int", i.second.getVaule<int>());
						lua_settable(L, -3);
						break;
					case TagType::Int64:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Int64", i.second.getVaule<__int64>());
						lua_settable(L, -3);
						break;
					case TagType::Float:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Float", i.second.getVaule<float>());
						lua_settable(L, -3);
						break;
					case TagType::Double:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "Double", i.second.getVaule<double>());
						lua_settable(L, -3);
						break;
					case TagType::String:
						lua_pushstring(L, i.first.c_str());
						setNode(L, "String", i.second.getVaule<string>().c_str());
						lua_settable(L, -3);
						break;
					case TagType::ByteArray:
						break;
					case TagType::List:
						try {
							lua_pushstring(L, i.first.c_str());
							tag2List(L, &i.second);
							lua_settable(L, -3);
						}
						catch (std::string e) {
							cout << e << endl;
						}
						break;
					case TagType::Compound:
						lua_pushstring(L, i.first.c_str());
						i.second.deserializeToLuaTable(L);
						lua_settable(L, -3);
						break;
					}
				}
			}
			lua_settable(L, -3);
		}
	}

    static Tag* toTag(luabridge::LuaRef table) {
		auto tag = Tag::createTag(TagType::Compound);
		auto nbt = luabridge::Iterator::Iterator(table["vaule"], false);
		while(!nbt.isNil()) {
			auto v = nbt.value();
			switch (H(v["type"].tostring()))
			{
			case H("Byte"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<char>());
				break;
			case H("Short"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<short>());
				break;
			case H("Int"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<int>());
				break;
			case H("Int64"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<__int64>());
				break;
			case H("Float"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<float>());
				break;
			case H("String"):
				tag->put(nbt.key().tostring(), v["vaule"].cast<string>());
				break;
			case H("List"):
			{
				auto t = list2Tag(v["vaule"]);
				tag->put(nbt.key().tostring(), t); 
			}
				break;
			case H("Compound"):
			{
				auto t = toTag(v);
				tag->put(nbt.key().tostring(), &t);
			}
				break;
			default:
				break;
			}
			nbt.next();
		}
		return tag;
	}

};

int Item_getNBT(lua_State* L) {
	lua_checkstack(L, 15);
	auto item = (uintptr_t)lua_tointeger(L, 2);
	Tag* tag = 0;
	SymCall("?save@ItemStackBase@@QEBA?AV?$unique_ptr@VCompoundTag@@U?$default_delete@VCompoundTag@@@std@@@std@@XZ",
		void*, void*, Tag**)((void*)item, &tag);
	tag->deserializeToLuaTable(L);
	return 1;
}

int Block_getNBT(lua_State* L) {
	auto block = (uintptr_t)lua_tointeger(L, 2);
	((Tag*)(block + 24))->deserializeToLuaTable(L);
	return 1;
}

int Actor_getNBT(lua_State* L) {
	auto ac = (uintptr_t)lua_tointeger(L, 2);
	auto tag = Tag::createTag(TagType::Compound);
	SymCall("?save@Actor@@UEAA_NAEAVCompoundTag@@@Z",
		void, void*, void*)((void*)ac, tag);
	tag->deserializeToLuaTable(L);
	return 1;
}

addApiEx(Actor_setNBT, {
	auto t = luabridge::LuaRef::fromStack(L,2);
    auto tag = Tag::toTag(t);
	tag->deserializeToLuaTable(L);
	return 1;
})

addApiEx(Item_setNBT, {
	auto itm = (uintptr_t)lua_tointeger(L, 2);
	auto t = luabridge::LuaRef::fromStack(L,3);
	auto tag = Tag::toTag(t);
	SymCall("?fromTag@ItemStack@@SA?AV1@AEBVCompoundTag@@@Z",
		void, void*, void*)((void*)itm, tag);
	return 0;
})
static const luaL_Reg R_NBT[] = {
	{"fromItem",Item_getNBT},
	{"fromBlock",Block_getNBT},
	{"fromActor",Actor_getNBT},
	{"setActor",Actor_setNBT},
	{"setItem",Item_setNBT},
	{ NULL,NULL }
};

void initNBTM() {
	registerLuaModule(LModule([](lua_State* L) {
		lua_newtable(L);
		luaL_setfuncs(L, R_NBT, 0);
		lua_setglobal(L, "NBT");
		}));
}