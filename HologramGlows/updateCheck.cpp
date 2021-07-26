#include "pch.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include<rapidjson/rapidjson.h>
#include<rapidjson/document.h>

int updateCheck() {
	std::thread t([] {
		httplib::Client cli("https://gitee.com");
		cli.set_connection_timeout(0, 300000);
		auto res = cli.Get("/dreamguxiang/LLessentials/raw/master/llupdate.json");
		rapidjson::Document json;
		if (res) {
			json.Parse(res->body.c_str());
			if (json.HasParseError()) {
				std::cout << u8"HologramGlows > 远程json格式出错，请联系作者" << std::endl;
				return;
			}
			auto arr = json.GetArray();
			auto Latest_release = arr[arr.Size() - 1]["name"].GetString();
			auto Latest_message = arr[arr.Size() - 1]["message"].GetString();
			auto Latest_message_zh = arr[arr.Size() - 1]["message_zh"].GetString();
			auto forceupdate = arr[arr.Size() - 1]["forceupdate"].GetInt();
			auto forceupdateall = arr[arr.Size() - 1]["forceupdateall"].GetBool();
			if (Latest_release != version) {
				int now = atoi(version.c_str());
				int yc = atoi(string(Latest_release).c_str());
				if (now - yc > 0) {
					std::cout << u8"HologramGlows > 当前为预览版：" << version << u8"，最新稳定版：" << Latest_release << std::endl;
				}
				else if (forceupdate == now)
				{
					std::cout << u8"HologramGlows > 你当前使用的版本（" << version << u8"）出现严重bug，本版本为强制更新!"
						<< u8"\nHologramGlows > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nHologramGlows > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nHologramGlows > DOWNLOAD LINK:https://www.minebbs.com/resources/hologramglows.2344/" << std::endl;
					std::cout << u8"5秒后自动退出" << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else if (forceupdateall == true) {
					std::cout << u8"HologramGlows > 你收到一个全版本强制更新！请及时更新!"
						<< u8"\nHologramGlows > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nHologramGlows > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nHologramGlows > DOWNLOAD LINK:https://www.minebbs.com/resources/hologramglows.2344/" << std::endl;
					std::cout << u8"5秒后自动退出" << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else{
					std::cout << u8"HologramGlows > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nHologramGlows > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nHologramGlows > DOWNLOAD LINK:https://www.minebbs.com/resources/hologramglows.2344/" << std::endl;
				}
			}
			else {
				std::cout << u8"HologramGlows > 当前版本为最新版：" << Latest_release
					<< "\nHologramGlows > The current version is the latest version: " << Latest_release << std::endl;
			}
		}
		else
		{
			std::cout << u8"[LLEssentials]获取更新失败，请检测你的网络或远程服务器出现访问障碍\n[LLEssentials]Failed to get update" << std::endl;
		}
		});
	t.detach();
}
