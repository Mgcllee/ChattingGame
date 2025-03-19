#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"
#include "ChatServer/public/OverlappedExpansion.h"

#include "ChatServer/public/Client.h"


static std::mutex mutex_login_user_list;

class NetworkManagerGrain : public IGrain
{
public:
	NetworkManagerGrain();
	virtual ~NetworkManagerGrain();

	virtual void packet_worker(HANDLE h_iocp,
		SOCKET server_socket, SOCKET accept_client_socket, OverlappedExpansion* accept_overlapped_expansion) override;

	inline static std::unordered_set<std::wstring> login_users;
	inline static std::unordered_map<int, Client> clients;
	inline static std::unordered_map<int, Client> LogViewers;

private:

	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;

	void construct_receive_packet(int client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(int player_ticket, wchar_t* packet);
};

