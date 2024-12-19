#pragma once

#include "OverlappedExpansion.h"

class Client : private OverlappedExpansion
{
private:

public:
	Client() {}
	Client(SOCKET in_socket) : client_socket(in_socket) { }
	SOCKET client_socket;

	void send_packet(char* packet);
	void recv_packet();
};