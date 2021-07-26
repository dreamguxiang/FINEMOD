
// 注意: 本项目的所有源文件都必须是 UTF-8 编码
#define MIRAICPP_STATICLIB
#include <iostream>
#include <map>
#include <mirai.h>
using namespace std;
using namespace Cyan;
#include "pch.h"
#include <api/LiteLoaderApi.h>

int entry()
{
	MiraiBot bot;
	SessionOptions opts;
	opts.BotQQ.Set(778114152);
    opts.HttpHostname = "localhost";
	opts.EnableVerify = false;
	opts.HttpPort = 8080;
	opts.SingleMode = true;
	opts.ThreadPoolSize = 32;
	while (true)
	{
		try
		{
			cout << "尝试与 mirai-api-http 建立连接..." << endl;
			bot.Connect(opts);
			break;
		}
		catch (const std::exception& ex)
		{
			cout << ex.what() << endl;
		}
		MiraiBot::SleepSeconds(1);
	}
	cout << "Bot Working..." << endl;


	bot.On<GroupMessage>(
		[&](GroupMessage m)
		{
			try
			{
				if (int64_t(m.Sender.Group.GID) == 981460659) {
					string msg = m.MessageChain.GetPlainText();
					if (m.Sender.Permission == GroupPermission::Owner) {
						if (*msg.begin() == '%')
						{
							string cmd(msg, 1);
							auto out = liteloader::runcmdEx(cmd);
							if (out.first == 0) {
								m.Reply(MessageChain().Plain("执行失败"));
								return;
							}
							else {
								m.Reply(MessageChain().Plain("执行成功\n指令返回:" + out.second));
								return;
							}
						}
					}
					if (msg.find("%") != string::npos) {}
				}
			}
			catch (const std::exception& ex)
			{
				cout << ex.what() << endl;
			}
		});

	string cmd;
	while (cin >> cmd)
	{
		if (cmd == "exit")
		{
			bot.Disconnect();
			break;
		}
	}

	bot.On<LostConnection>([&](LostConnection e)
		{
			cout << e.ErrorMessage << " (" << e.Code << ")" << endl;
			while (true)
			{
				try
				{
					cout << "尝试与 mirai-api-http 重新建立连接..." << endl;
					bot.Reconnect();
					break;
				}
				catch (const std::exception& ex)
				{
					cout << ex.what() << endl;
				}
				MiraiBot::SleepSeconds(1);
			}
			cout << "成功与 mirai-api-http 重新建立连接!" << endl;
		});
	return 0;
}
