#pragma once

#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/Grain/NetworkManagerGrain.h"

class NetworkWorkerGrain : public IGrain
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

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};