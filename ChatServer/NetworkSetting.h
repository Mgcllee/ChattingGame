#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#define BUF_SIZE 1024
#define SERVER_PORT 9785

class NetworkSetting {
private:


public:
	NetworkSetting() {};
	~NetworkSetting() {};


};