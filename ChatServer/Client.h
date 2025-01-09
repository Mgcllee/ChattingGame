#pragma once

#include <string>
#include "OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
private:

public:
	SOCKET client_socket;
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	void send_packet(void* packet);
	void recv_packet();

public:
	std::wstring id;
	std::wstring pw;
};