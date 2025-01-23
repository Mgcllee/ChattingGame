#pragma once

#include "stdafx.h"
#include "Client.h"

class ChatRoomSession
{
private:

public:
	std::wstring room_name;

	ChatRoomSession(std::wstring new_room_name);
	~ChatRoomSession() {};

	void enter_room(Client& new_client);

	void sync_all_chat() {};
	void sync_one_chat() {};
};

