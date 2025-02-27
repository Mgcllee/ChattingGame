#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/Client.h"

static std::mutex mutex_login_user_list;

class ClientWorkerGrain : public IGrain
{
public:
	ClientWorkerGrain();
	virtual ~ClientWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	inline static std::unordered_set<std::wstring> login_users;
	inline static std::unordered_map<int, Client> clients;
	inline static std::unordered_map<int, Client> LogViewers;
	
	HANDLE h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database;

	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
	
	void construct_receive_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(int player_ticket, wchar_t* packet);
};