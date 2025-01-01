#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "Common/Packet.h"

enum SOCKET_TYPE { ACCEPT, RECV, SEND, LOGOUT };

class OverlappedExpansion {
public:
	OverlappedExpansion();
	OverlappedExpansion(char* packet);
	~OverlappedExpansion();

public:
	WSAOVERLAPPED overlapped;
	WSABUF wsa_buffer;
	char packet_buffer[BUF_SIZE];
	SOCKET_TYPE socket_type;
	int remain_packet_size;
};
