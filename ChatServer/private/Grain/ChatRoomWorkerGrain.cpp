#pragma once

#include "ChatServer/public/Grain/ChatRoomWorkerGrain.h"

ChatRoomWorkerGrain::ChatRoomWorkerGrain()
{
}

ChatRoomWorkerGrain::~ChatRoomWorkerGrain()
{
}

void ChatRoomWorkerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto [h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	while (true) {
		BOOL GQCS_result = GetQueuedCompletionStatus(h_iocp_chatroom, &num_bytes, &key, &overlapped, static_cast<DWORD>(INFINITY));
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (false == is_exist_GQCS_result(exoverlapped, GQCS_result)) {
			continue;
		}

		switch (exoverlapped->overlapped_type) {
		case OVERLAPPED_TYPE::REQUEST_JOIN_ROOM: {
			std::wstring room_name = reinterpret_cast<wchar_t*>(exoverlapped->packet_buffer);

			if (room_list.find(room_name) == room_list.end()) {
				room_list[room_name] = ChatRoomSession(room_name);
			}

			room_list[room_name].join_client(exoverlapped->user_id);
			break;
		}
		}
	}
}

bool ChatRoomWorkerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	if (exoverlapped == nullptr) return false;
	if (FALSE == GQCS_result) {
		// TODO: add error type log
		return false;
	}

	return true;
}