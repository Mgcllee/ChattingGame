#pragma once

#include "ChatServer/public/Grain/DataBaseWorkerGrain.h"

DataBaseWorkerGrain::DataBaseWorkerGrain()
{
}

DataBaseWorkerGrain::~DataBaseWorkerGrain()
{
}

void DataBaseWorkerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto [h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp_database, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* dboverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(reinterpret_cast<OverlappedExpansion*>(overlapped), GQCS_result)) {
			continue;
		}

		switch (dboverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::PRINT_CHAT_LOG: {
			wchar_t* chat_log = reinterpret_cast<wchar_t*>(dboverlapped->packet_buffer);
			std::wprintf(L"%s\n", chat_log);
		}
		}
	}
}

bool DataBaseWorkerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		// TODO: add error type log
		return false;
	}

	return true;
}

