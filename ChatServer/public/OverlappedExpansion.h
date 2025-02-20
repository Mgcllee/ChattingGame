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

	, REQUEST_MAKE_ROOM
	, REQUEST_JOIN_ROOM
	, REQUEST_LEAVE_ROOM

	, REQUEST_CHAT_LOG
	
	, PRINT_CHAT_LOG
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

struct DBOverlapped : WSAOVERLAPPED {
	DBOverlapped(std::wstring in_chat, OVERLAPPED_TYPE type)
		: chat_log(in_chat) 
		, overlapped_type(type)
	{ 
		
	}

	OVERLAPPED_TYPE overlapped_type;
	std::wstring chat_log;
};
