#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/OverlappedExpansion.h"

class NetworkManagerGrain : public IGrain
{
protected:
	SOCKET server_socket;
	SOCKET accept_client_socket;
	OverlappedExpansion accept_overlapped_expansion;

public:
	NetworkManagerGrain(std::string accpet_addr, HANDLE& h_iocp_network);

	std::tuple<SOCKET&, SOCKET&, OverlappedExpansion*> get_socket_variable();
	
	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};

