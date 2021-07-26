#include "pch.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include "Helper.h"
int updateCheck() {
	std::thread t([] {
		httplib::Client cli("https://gitee.com");
		cli.set_connection_timeout(0, 300000);
		auto res = cli.Get("/dreamguxiang/LLessentials/raw/master/update.json");
		rapidjson::Document json;
		if (res) {
			json.Parse(res->body.c_str());
			if (json.HasParseError()) {
				cout << u8"LLEssentials > 远程json格式出错，请联系作者" << endl;
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
					cout << u8"LLEssentials > 当前为预览版：" << version << u8"，最新稳定版：" << Latest_release << endl;
				}
				else if (forceupdate == now)
				{
					cout << u8"LLEssentials > 你当前使用的版本（" << version << u8"）出现严重bug，本版本为强制更新!"
						<< u8"\nLLEssentials > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
					cout << u8"5秒后自动退出" << endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else if (forceupdateall == true) {
					cout << u8"LLEssentials > 你收到一个全版本强制更新！请及时更新!"
						<< u8"\nLLEssentials > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
					cout << u8"5秒后自动退出" << endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else{
					cout << u8"LLEssentials > 有一个新的版本更新：" << Latest_release << u8"，更新日志：" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
				}
			}
			else {
				cout << u8"LLEssentials > 当前版本为最新版：" << Latest_release
					<< "\nLLEssentials > The current version is the latest version: " << Latest_release << endl;
			}
		}
		else
		{
			cout << u8"LLEssentials > 获取更新失败，请检测你的网络或远程服务器出现访问障碍\n[LLEssentials]Failed to get update" << endl;
		}
		});
	t.detach();
}
