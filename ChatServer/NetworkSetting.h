#pragma once

#include "Packet.h"

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

class NetworkSetting {
private:


public:
	NetworkSetting();
	~NetworkSetting() {};


};