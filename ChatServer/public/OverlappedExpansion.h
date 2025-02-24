#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <string>
#include "Common/Packet.h"

enum OVERLAPPED_TYPE { 
	CLIENT_ACCEPT
	, PACKET_RECV
	, PACKET_SEND
	, LOGOUT 

	, REQUEST_JOIN_ROOM
	, RESPONSE_JOIN_ROOM

	, REQUEST_LEAVE_ROOM

	, REQUEST_CHAT_LOG
	
	, PRINT_CHAT_LOG
	, CHECK_EXIST_CLIENTS
};

struct OverlappedExpansion {
	OverlappedExpansion();
	OverlappedExpansion(short* packet);
	~OverlappedExpansion();

	WSAOVERLAPPED overlapped;
	WSABUF wsa_buffer;
	OVERLAPPED_TYPE overlapped_type;
	int remain_packet_size;
	unsigned short packet_buffer[BUF_SIZE];
	std::wstring user_id;
};