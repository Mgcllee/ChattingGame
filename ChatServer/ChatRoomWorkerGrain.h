#pragma once

#include "IGrain.h"

class ChatRoomWorkerGrain : public IGrain
{
	// h_iocp_chatroom ���
	std::unordered_map<std::wstring, ChatRoomSession> room_list;
	// ===================
};

