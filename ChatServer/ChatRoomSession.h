#pragma once

#include "stdafx.h"
#include "Client.h"

class ChatRoomSession
{
private:

public:
	ChatRoomSession(std::vector<Client> clients) {} ;
	~ChatRoomSession() {};

	void sync_all_chat() {};
	void sync_one_chat() {};
};

