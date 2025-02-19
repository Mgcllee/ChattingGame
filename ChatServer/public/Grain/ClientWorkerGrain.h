#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/Client.h"

class ClientWorkerGrain : public IGrain
{
private:
	inline static std::unordered_map<int, Client> clients;
	
public:
	ClientWorkerGrain();
	~ClientWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;

	void construct_receive_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(int player_ticket, short* packet);
};