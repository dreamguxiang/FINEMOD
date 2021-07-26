#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
extern unsigned short port;

#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

#define listenPort 25836
#define serverAddress "121.4.163.104"

namespace ENCIO {
	void DECRYPT(void const* ptr, unsigned long long key, void* out, size_t sz) {
		unsigned char const* view = (unsigned char const*)ptr;
		unsigned char IV = key >> 4;
		unsigned char* ovi = (unsigned char*)out;
		for (size_t i = 0; i < sz; ++i) {
			unsigned char NOW = *view;
			++view;
			unsigned char mirror = *view;
			++view;
			unsigned char DEC = NOW ^ key;
			unsigned char gmir = NOW ^ IV;
			//std::cout << int(mirror) << "," << int(gmir) << std::endl;
			if (gmir != mirror) {
				//printf("at pos %d %d\n", i, sz);
				throw std::exception();
			}
			IV *= 131;
			key *= 19260817;
			*ovi = DEC;
			ovi++;
		}
	}
};

void checkAuthorize() {
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//向服务器发起请求
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;
	inet_pton(AF_INET, serverAddress, &sockAddr.sin_addr.s_addr);
	sockAddr.sin_port = htons(listenPort);
	short retry = 0;
	while (connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1 && retry <= 3) {
		std::cout << "[LiteLand] Failed to connect\n";
		if (retry >= 3) {
			std::cout << "[LiteLand] Unauthorized\n";
			exit(0);
		}
		retry++;
	}
	//与服务端握手
	//std::cout << "start connection\n";
	char request[128] = "AuthorizeRequest114514";
	send(sock, request, strlen(request), NULL);
	Sleep(1);//暂停1ms

	char portStr[128] = "";
	//向服务器发送port
	_itoa_s(port, portStr, 128, 10);
	send(sock, portStr, strlen(portStr), NULL);
	//接收服务器传回的数据
	char szBuffer[257] = { 0 };
	recv(sock, szBuffer, 257, NULL);//接收Key

	char szBuffer2[257] = { 0 };
	recv(sock, szBuffer2, 257, NULL);//接收返回值
	//输出接收到的数据
	//std::cout << "receive\n";
	char decryptKey[128] = "";
	ENCIO::DECRYPT(szBuffer, 1145141919, decryptKey, 128);//解密得到Key
	unsigned long long decryptKey_ = std::stoull(decryptKey);//将char Key转换为unsigned long long
	char decryptCode[128] = "";
	ENCIO::DECRYPT(szBuffer2, decryptKey_, decryptCode, 128);//使用Key解密返回值
	int code = std::stoi(decryptCode);
	if (code / 2 != 959905 + port / 2) {
		std::cout << "[LiteLand] Unauthorized\n";
		exit(0);
	}
	else {
		std::cout << "[LiteLand] Authorized\n";
	}
	//关闭套接字
	closesocket(sock);
	//终止使用 DLL
	WSACleanup();
}