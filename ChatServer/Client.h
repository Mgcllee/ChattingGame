#pragma once

#include "OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
private:
	SOCKET client_socket;

public:
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	void send_packet(char* packet);
	void recv_packet();
};