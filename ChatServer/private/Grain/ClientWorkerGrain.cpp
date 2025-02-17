#pragma once

#include "ChatServer/public/Grain/ClientWorkerGrain.h"

ClientWorkerGrain::ClientWorkerGrain()
{
}

ClientWorkerGrain::~ClientWorkerGrain()
{
}

void ClientWorkerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto [h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp_clients, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(exoverlapped, GQCS_result)) {
			continue;
		}

		switch (exoverlapped->overlapped_type) {
			OVERLAPPED_TYPE::CLIENT_ACCEPT: {
				// TODO: Redis 사용
				// -> 문제 발생: Redis는 기본적으로 Windows를 지원하지 않음.
				// --> 대책 강구 필요
				break;
			}
		}
	}
}

bool ClientWorkerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		// TODO: add error type log
		return false;
	}

	return true;
}

