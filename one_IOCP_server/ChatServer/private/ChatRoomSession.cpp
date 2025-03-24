#pragma once

#include "ChatServer/public/ChatRoomSession.h"

ChatRoomSession::ChatRoomSession()
{
}

ChatRoomSession::ChatRoomSession(std::wstring new_room_name)
	: room_name(new_room_name)
{
}

ChatRoomSession::~ChatRoomSession()
{
}

void ChatRoomSession::join_client(std::wstring new_user_id)
{
	if (members_id.find(new_user_id) == members_id.end()) {
		members_id.insert(new_user_id);
	}
}
