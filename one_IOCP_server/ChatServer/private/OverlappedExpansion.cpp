#pragma once

#include "ChatServer/public/OverlappedExpansion.h"

OverlappedExpansion::OverlappedExpansion() 
	: overlapped_type(OVERLAPPED_TYPE::PACKET_RECV)
	, wsa_buffer(WSABUF(MAX_PACKET_SIZE, reinterpret_cast<CHAR*>(packet_buffer))) 
	, remain_packet_size(0)
{
	ZeroMemory(&overlapped, sizeof(overlapped));
}

OverlappedExpansion::OverlappedExpansion(wchar_t* packet)
	: overlapped_type(OVERLAPPED_TYPE::PACKET_SEND)
	, wsa_buffer(WSABUF(packet[0], reinterpret_cast<CHAR*>(packet_buffer)))
	, remain_packet_size(0)
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	memcpy(packet_buffer, packet, packet[0]);
}

OverlappedExpansion::~OverlappedExpansion()
{
}

