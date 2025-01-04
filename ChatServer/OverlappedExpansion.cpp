#pragma once

#include "OverlappedExpansion.h"

OverlappedExpansion::OverlappedExpansion() {
	wsa_buffer.len = BUF_SIZE;
	wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);;
	socket_type = SOCKET_TYPE::RECV;
	ZeroMemory(&overlapped, sizeof(overlapped));
	remain_packet_size = 0;
}

OverlappedExpansion::OverlappedExpansion(short* packet) {
	socket_type = SOCKET_TYPE::RECV;
	wsa_buffer.len = packet[0];
	wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);;
	ZeroMemory(&overlapped, sizeof(overlapped));
	socket_type = SEND;
	memcpy(packet_buffer, packet, packet[0]);
	remain_packet_size = 0;
}

OverlappedExpansion::~OverlappedExpansion()
{
}

