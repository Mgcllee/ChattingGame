#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "Common/Packet.h"

enum SOCKET_TYPE { ACCEPT, RECV, SEND, LOGOUT };

class OverlappedExpansion {
public:
	OverlappedExpansion() { 
		wsa_buffer.len = BUF_SIZE;
		wsa_buffer.buf = packet_buffer;
		socket_type = SOCKET_TYPE::RECV;
		ZeroMemory(&overlapped, sizeof(overlapped));
		remain_packet_size = 0;
	}
	OverlappedExpansion(char* packet) { 
		socket_type = SOCKET_TYPE::RECV; 
		wsa_buffer.len = packet[0];
		wsa_buffer.buf = packet_buffer;
		ZeroMemory(&overlapped, sizeof(overlapped));
		socket_type = SEND;
		memcpy(packet_buffer, packet, packet[0]);
		remain_packet_size = 0;
	}
	~OverlappedExpansion() {};

public:
	WSAOVERLAPPED overlapped;
	WSABUF wsa_buffer;
	char packet_buffer[BUF_SIZE];
	SOCKET_TYPE socket_type;
	int remain_packet_size;
};
