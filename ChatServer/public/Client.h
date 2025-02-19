#pragma once

#include <string>
#include "ChatServer/public/OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
public:
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	void send_packet(void* packet);
	void recv_packet();

	std::wstring id;
	std::wstring pw;

private:
	SOCKET client_socket;
};