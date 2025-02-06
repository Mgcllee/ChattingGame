#pragma once

#include "stdafx.h"
#include "Client.h"

class ChatRoomSession
{
private:
	std::unordered_map<std::wstring, Client&> room_member_id;

public:
	std::wstring room_name;

	ChatRoomSession(std::wstring new_room_name);
	~ChatRoomSession() {};

	void join_client(Client& new_client);

	void sync_all_chat() {};
	void sync_one_chat() {};
};

