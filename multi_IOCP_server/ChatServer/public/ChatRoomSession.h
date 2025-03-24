#pragma once

#include "ChatServer/public/stdafx.h"
#include "ChatServer/public/OverlappedExpansion.h"

class ChatRoomSession
{
public:
	ChatRoomSession();
	ChatRoomSession(std::wstring new_room_name);
	~ChatRoomSession();

	void join_client(std::wstring new_user_id);

	void sync_all_chat() {};
	void sync_one_chat() {};

private:
	std::wstring room_name;
	std::unordered_set<std::wstring> members_id;
};

