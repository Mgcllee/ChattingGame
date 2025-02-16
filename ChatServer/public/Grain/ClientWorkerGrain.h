#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/Client.h"

class ClientWorkerGrain : public IGrain
{
private:
	std::atomic<int> ticket_number;

	// TODO: client login/logout log는 redis(in memory) 사용해보기
	// std::unordered_map<int, Client> clients;
	
public:
	ClientWorkerGrain();
	~ClientWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};