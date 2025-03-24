#pragma once

#include <tuple>
#include "ChatServer/public/OverlappedExpansion.h"

class IGrain {
public:
	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) = 0;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) = 0;
	
	void post_exoverlapped(
		HANDLE h_iocp
		, wchar_t packet_buffer[]
		, std::wstring user_id
		, OVERLAPPED_TYPE overlapped_type
	) 
	{
		DWORD bytes = sizeof(OverlappedExpansion);
		OverlappedExpansion* exoverlapped = new OverlappedExpansion();
		exoverlapped->overlapped_type = overlapped_type;
		wcscpy_s(exoverlapped->packet_buffer, packet_buffer);
		exoverlapped->user_id = user_id;
		PostQueuedCompletionStatus(h_iocp, bytes, 0, &exoverlapped->overlapped);
	}
};