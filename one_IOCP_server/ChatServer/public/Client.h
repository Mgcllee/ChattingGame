#pragma once

#include <string>
#include "ChatServer/public/OverlappedExpansion.h"

class Client : public OverlappedExpansion
{
public:
	Client() : client_socket(NULL) { }
	Client(SOCKET in_socket) : client_socket(in_socket) { }

	DWORD send_packet(void* packet);
	DWORD recv_packet();
	void disconnect_server();

	std::wstring id;
	std::wstring pw;

private:
	SOCKET client_socket;

};