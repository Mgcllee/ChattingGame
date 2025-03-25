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
	inline static std::unordered_map<HANDLE, Client> clients;
	inline static std::unordered_map<HANDLE, Client> LogViewers;

private:
	// inline static std::atomic<int> accpet_ticket // -> 단순 증가일 뿐, 중간 삭제가 어려움

	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;

	void construct_receive_packet(HANDLE client_ticket, OverlappedExpansion* exoverlapped, DWORD num_bytes);
	void process_packet(HANDLE player_ticket, wchar_t* packet);
};

