#pragma once

#include "stdafx.h"
#include "IGrain.h"
#include "Client.h"

class ClientWorkerGrain : public IGrain
{
private:
	std::atomic<int> ticket_number;

	// TODO: client login/logout log�� redis(in memory) ����غ���
	// std::unordered_map<int, Client> clients;
	
public:
	ClientWorkerGrain();
	~ClientWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};