#include "pch.h"
#include <iostream>
#include <string>
#include <regex>
#include <stdio.h>
#include <string.h>
#include <lbpch.h>
#include <loader/Loader.h>
#include <string>
#include <JsonLoader.h>
#include <chrono>
#include <fstream>
#include <mc/Player.h>
#include <stl\varint.h>
#include <api\xuidreg\xuidreg.h>
#include <api/LiteLoaderApi.h>
#include <mc/Item.h>
using namespace std::filesystem;
using namespace std;
/*
THook(void, "?log@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4ZZ",
	int a1, int a2, __int64 a3, int a4, int a5, __int64 a6, int a7, __int64 a8, ...) {
	va_list va;
	string mode = (char*)a6;
	auto text = (char*)a8;
	//std::cout << mode << std::endl;
	//std::cout << text << std::endl;
    //std::cout << a7 << std::endl;
	va_start(va, a8);

	switch (a7) {
	    case 178+8: {
		   // const char* text = u8"正在设置服务器日志记录......";
		    return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
			    void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
		    break;
	    }
		case 236+8: {
			//const char* text = u8"启动服务器中";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 237+8: {
			//const char* text = u8"版本 %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 238+8: {
			//const char* text = u8"会话ID %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 250+8: {
			//const char* text = u8"地图名称 %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 275+8: {
			//const char* text = u8"游戏模式 %d %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 277+8: {
			//const char* text = u8"游戏难度 %d %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 403+8: {
			//const char* text = u8"已启用内容日志记录。正在将日志写入：%s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 193: {
			//const char* text = u8"正在打开 %s";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break; 
		}
		case 147: {
			auto ts = va_arg(va, __int64);
			string input = (char*)ts;
			std::string output = std::regex_replace(input,std::regex("[^0-9]*([0-9]+)"),std::string("$1"));
			output.erase(0, 1);
			//string text = u8"支持IPV4协议，端口："+ output;
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text);
			break;
		}
		case 158: {
			auto ts = va_arg(va, __int64);
			string input = (char*)ts;
			std::string output = std::regex_replace(input, std::regex("[^0-9]*([0-9]+)"), std::string("$1"));
			output.erase(0, 1);
			//string text = u8"支持IPV6协议，端口：" + output;
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text);
			break;
		}
		case 572+8: {
			//const char* text = u8"服务器启动完毕";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 1027+8: {
			const char* text = u8"玩家: %s, xuid: %s 断开连接";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 1446+8: {
			const char* text = u8"玩家: %s, xuid: %s 加入游戏";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 128+8: {
			const char* text = u8"已请求服务器停止";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 641+8: {
			const char* text = u8"正在关闭服务器......";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		case 1231+8: {
			const char* text = u8"正在计划自动压缩......";
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, (__int64)text, (__int64)va);
			break;
		}
		default:
			return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
				void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, a8, (__int64)va);
	}
	return SymCall("?log_va@BedrockLog@@YAXW4LogCategory@1@V?$bitset@$02@std@@W4LogRule@1@W4LogAreaID@@IPEBDH4PEAD@Z",
		void, unsigned int, unsigned int, int, int, unsigned int, __int64, __int64, __int64, __int64)(a1, a2, a3, a4, a5, a6, a7, a8, (__int64)va);
}

*/
class Experiments;
class LevelData;
bool Gametest, Scripting, DataDrivenItems, DataDrivenBiomes, CavesAndCliffs, UpcomingCreatorFeatures;
void loadCfg() {
	create_directory("plugins\\LLEssentials");
	try {
		ConfigJReader jr("plugins/openExperiments/Experiments.json");
		jr.bind("Gametest", Gametest, false);
		jr.bind("Scripting", Scripting, false);
		jr.bind("DataDrivenItems", DataDrivenItems, false);
		jr.bind("DataDrivenBiomes", DataDrivenBiomes, false);
		jr.bind("UpcomingCreatorFeatures", UpcomingCreatorFeatures, false);
		jr.bind("CavesAndCliffs", CavesAndCliffs, false);
	}
	catch (string e) {
		printf("[openExperiments] json error %s\n", e.c_str());
		throw 0;
	}
}


