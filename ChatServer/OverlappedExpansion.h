#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "Common/Packet.h"

enum OVERLAPPED_TYPE { 
	CLIENT_ACCEPT
	, PACKET_RECV
	, PACKET_SEND
	, LOGOUT 

	
};

class OverlappedExpansion {
public:
	OverlappedExpansion();
	OverlappedExpansion(short* packet);
	~OverlappedExpansion();

public:
	WSAOVERLAPPED overlapped;
	WSABUF wsa_buffer;
	short packet_buffer[BUF_SIZE];
	OVERLAPPED_TYPE overlapped_type;
	int remain_packet_size;
};
