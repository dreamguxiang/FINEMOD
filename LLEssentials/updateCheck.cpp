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
				cout << u8"LLEssentials > Զ��json��ʽ��������ϵ����" << endl;
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
					cout << u8"LLEssentials > ��ǰΪԤ���棺" << version << u8"�������ȶ��棺" << Latest_release << endl;
				}
				else if (forceupdate == now)
				{
					cout << u8"LLEssentials > �㵱ǰʹ�õİ汾��" << version << u8"����������bug�����汾Ϊǿ�Ƹ���!"
						<< u8"\nLLEssentials > ��һ���µİ汾���£�" << Latest_release << u8"��������־��" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
					cout << u8"5����Զ��˳�" << endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else if (forceupdateall == true) {
					cout << u8"LLEssentials > ���յ�һ��ȫ�汾ǿ�Ƹ��£��뼰ʱ����!"
						<< u8"\nLLEssentials > ��һ���µİ汾���£�" << Latest_release << u8"��������־��" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
					cout << u8"5����Զ��˳�" << endl;
					std::this_thread::sleep_for(std::chrono::seconds(5));
					exit(0);
				}
				else{
					cout << u8"LLEssentials > ��һ���µİ汾���£�" << Latest_release << u8"��������־��" << Latest_message_zh
						<< "\nLLEssentials > There is a new version: " << Latest_release << ",Update log: " << Latest_message
						<< "\nLLEssentials > DOWNLOAD LINK:https://www.minebbs.com/resources/llessentials.2384/" << endl;
				}
			}
			else {
				cout << u8"LLEssentials > ��ǰ�汾Ϊ���°棺" << Latest_release
					<< "\nLLEssentials > The current version is the latest version: " << Latest_release << endl;
			}
		}
		else
		{
			cout << u8"LLEssentials > ��ȡ����ʧ�ܣ�������������Զ�̷��������ַ����ϰ�\n[LLEssentials]Failed to get update" << endl;
		}
		});
	t.detach();
}
