#pragma once

#include "ChatServer/public/OverlappedExpansion.h"

OverlappedExpansion::OverlappedExpansion() {
	overlapped_type = OVERLAPPED_TYPE::PACKET_RECV;
	ZeroMemory(&overlapped, sizeof(overlapped));
	wsa_buffer.len = BUF_SIZE;
	ZeroMemory(&packet_buffer, sizeof(packet_buffer));
	wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);
	remain_packet_size = 0;
}

OverlappedExpansion::OverlappedExpansion(short* packet) {
	overlapped_type = OVERLAPPED_TYPE::PACKET_SEND;
	ZeroMemory(&overlapped, sizeof(overlapped));
	memcpy(packet_buffer, packet, packet[0]);
	wsa_buffer.len = packet[0];
	wsa_buffer.buf = reinterpret_cast<CHAR*>(packet_buffer);
	remain_packet_size = 0;
}

OverlappedExpansion::~OverlappedExpansion()
{
}

