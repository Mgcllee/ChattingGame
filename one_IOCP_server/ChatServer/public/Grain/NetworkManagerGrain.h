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

	static inline std::unordered_set<std::wstring> login_users;
	static inline std::unordered_map<uint64_t, Client> clients;
	static inline std::unordered_map<uint64_t, Client> LogViewers;

private:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;

	void construct_receive_packet(HANDLE h_iocp, uint64_t client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(HANDLE h_iocp, uint64_t player_ticket, wchar_t* packet);
};

