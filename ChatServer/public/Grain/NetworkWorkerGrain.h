#pragma once

#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/Grain/NetworkManagerGrain.h"

class NetworkWorkerGrain : public IGrain
{
public:
	NetworkWorkerGrain(NetworkManagerGrain& networking);

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	SOCKET& server_socket;
	SOCKET& accept_client_socket;
	OverlappedExpansion* accept_overlapped_expansion;

	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};