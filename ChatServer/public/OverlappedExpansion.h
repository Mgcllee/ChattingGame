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
	, CHECK_EXIST_CLIENTS
};

struct OverlappedExpansion {
	OverlappedExpansion();
	OverlappedExpansion(short* packet);
	~OverlappedExpansion();

	WSAOVERLAPPED overlapped;
	WSABUF wsa_buffer;
	OVERLAPPED_TYPE overlapped_type;
	short packet_buffer[BUF_SIZE];
	int remain_packet_size;
};

struct DBOverlapped : OverlappedExpansion {
	DBOverlapped(std::wstring in_chat, OVERLAPPED_TYPE type)
		: chat_log(in_chat) 
		, overlapped_type(type)
	{ 
		
	}

	OVERLAPPED_TYPE overlapped_type;
	std::wstring chat_log;
};

struct TIMEROverlapped : OverlappedExpansion {
	OVERLAPPED_TYPE overlapped_type;
};