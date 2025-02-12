#pragma once

#include "NetworkManagerGrain.h"

class NetworkWorkerGrain
{
protected:
	SOCKET& server_socket;
	SOCKET& accept_client_socket;
	OverlappedExpansion* accept_overlapped_expansion;

public:
	NetworkWorkerGrain(NetworkManagerGrain& networking)
		: server_socket = std::get<0>(networking.get_socket_variable())
		, accept_client_socket = std::get<1>(networking.get_socket_variable())
		, accept_overlapped_expansion = std::get<2>(networking.get_socket_variable());

	void network_packet_worker(HANDLE& h_iocp_network, HANDLE& h_iocp_clients);

protected:
	bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result);
};