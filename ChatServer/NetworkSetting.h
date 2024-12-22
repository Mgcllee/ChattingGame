#pragma once

#include "Packet.h"
#include "OverlappedExpansion.h"

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

class NetworkSetting {
private:
	SOCKET server_socket;
	SOCKET accept_client_socket;

	OverlappedExpansion* accept_overlapped_expansion;
	HANDLE h_iocp;

public:
	NetworkSetting();
	~NetworkSetting();

	void create_iocp_handle();
	void accept_clients();
};