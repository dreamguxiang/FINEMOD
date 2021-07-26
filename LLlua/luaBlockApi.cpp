#include "pch.h"
#include "luaBindings.h"
#include <mc/Block.h>
#include <mc/BlockSource.h>
extern Minecraft* mc;

addApiEx(block_getId, {
	auto block = (BlockLegacy*)((uintptr_t)lua_tointeger(L, 2) + 16);
	lua_pushinteger(L, block->getBlockItemId());
	return 1;
	})

	addApiEx(block_getName, {
		auto block = (void*)(uintptr_t)lua_tointeger(L, 2);
		void* hashedstr = SymCall("?getName@Block@@QEBAAEBVHashedString@@XZ",
			void*, void*)(block);
		auto blockname = ((const char*)hashedstr + 8);
		lua_pushstring(L, blockname);
		return 1;
		})

		addApiEx(block_get, {
			auto x = lua_tonumber(L, 2);
			auto y = lua_tonumber(L, 3);
			auto z = lua_tonumber(L, 4);
			auto dimid = lua_tointeger(L, 5);
			auto dim = SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
				  uintptr_t, void*, int)(mc->getLevel(), dimid);
			auto bs = (BlockSource*)(dim + 88);
			auto blk = SymCall("?getBlock@BlockSource@@QEBAAEBVBlock@@HHH@Z",
				Block*, void*, int, int, int)(bs, x, y, z);
			lua_pushinteger(L, (uintptr_t)blk);
			return 1;
			})

			addApiEx(block_set, {
				auto blk = (void*)lua_tointeger(L, 2);
				auto x = (int)lua_tointeger(L, 3);
				auto y = (int)lua_tointeger(L, 4);
				auto z = (int)lua_tointeger(L, 5);
				auto dimid = lua_tointeger(L, 6);
				auto dim = SymCall("?getDimension@Level@@UEBAPEAVDimension@@V?$AutomaticID@VDimension@@H@@@Z",
					uintptr_t, void*, int)(mc->getLevel(), dimid);
				auto bs = (BlockSource*)(dim + 88);
				SymCall("?setBlock@BlockSource@@QEAA_NAEBVBlockPos@@AEBVBlock@@HPEBUActorBlockSyncMessage@@@Z",
					bool, void*, void*, const BlockPos&, int)(bs, blk, { x,y,z }, 0);
				return 0;
				})

				Block* Str2Block(string str) {
					string bstr = "?m" + str + "@VanillaBlocks@@3PEBVBlock@@EB";
					return *(Block**)dlsym_real(bstr.c_str());
				}

				addApiEx(block_createBlock, {
					auto blk = lua_tostring(L, 2);
					auto blk_p = Str2Block(blk);
					lua_pushinteger(L, (uintptr_t)blk_p);
					return 1;
					})

					static const luaL_Reg R_Item[] = {
						{"getName",block_getName},
						{"getId",block_getId},
						{"createBlock",block_createBlock},
						{"set",block_set},
						{"get",block_get},
						{ NULL,NULL }
					};

					void initBlockM() {
						registerLuaModule(LModule([](lua_State* L) {
							lua_newtable(L);
							luaL_setfuncs(L, R_Item, 0);
							lua_setglobal(L, "Block");
							}));
					}
