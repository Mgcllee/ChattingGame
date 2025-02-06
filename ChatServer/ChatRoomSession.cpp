#pragma once

#include "ChatRoomSession.h"

ChatRoomSession::ChatRoomSession(std::wstring new_room_name)
	: room_name(new_room_name)
{

}

void ChatRoomSession::join_client(Client& new_client)
{
	// TODO: thread safe (warning: data race condition!)
	// TODO: check exist member name or client (double)
	room_member.emplace_back(new_client);
}
