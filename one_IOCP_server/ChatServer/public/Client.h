#pragma once

#include <string>
#include "ChatServer/public/OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
public:
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	int send_packet(void* packet);
	int recv_packet();
	void disconnect_server();

	std::wstring id;
	std::wstring pw;

private:
	SOCKET client_socket;

};