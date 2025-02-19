#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/Interface/IGrain.h"

class ChatRoomWorkerGrain : public IGrain
{
public:
	ChatRoomWorkerGrain();
	~ChatRoomWorkerGrain();

	virtual void packet_worker(std::tuple<HANDLE, HANDLE, HANDLE, HANDLE> h_iocps) override;

private:
	// std::unordered_map<std::wstring, ChatRoomSession> room_list;

protected:
	virtual bool is_exist_GQCS_result(OverlappedExpansion* exoverlapped, BOOL GQCS_result) override;
};

