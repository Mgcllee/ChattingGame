#pragma once

#include "ChatServer/public/Grain/TimerGrain.h"
#include <thread>

TimerGrain::TimerGrain()
{
}

TimerGrain::~TimerGrain()
{
}

void TimerGrain::packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps)
{
	auto[h_iocp_network, h_iocp_clients, h_iocp_chatroom, h_iocp_database] = h_iocps;

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(10));
		
		OverlappedExpansion* timer_overlapped = new OverlappedExpansion();
		timer_overlapped->overlapped_type = OVERLAPPED_TYPE::CHECK_EXIST_CLIENTS;
		DWORD sent_byte = 0;
		ULONG_PTR key = 0;
		PostQueuedCompletionStatus(h_iocp_clients, sent_byte, key, &timer_overlapped->overlapped);
	}
}

bool TimerGrain::is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result)
{
	return false;
}