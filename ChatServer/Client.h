#pragma once

#include <string>
#include "OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
private:
	SOCKET client_socket;

public:
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	void send_packet(short* packet);
	void recv_packet();

public:
	std::wstring id;
	std::wstring pw;
};