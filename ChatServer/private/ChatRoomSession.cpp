#pragma once

#include "ChatServer/public/ChatRoomSession.h"

ChatRoomSession::ChatRoomSession(std::wstring new_room_name)
	: room_name(new_room_name)
{

}

void ChatRoomSession::join_client(Client& new_client)
{
	// TODO: thread safe (warning: data race condition!)
	// TODO: check exist member name or client (double)

	S2C_RESPONSE_JOIN_ROOM_PACK packet;
	packet.size = sizeof(packet);
	packet.type = S2C_PACKET_TYPE::RESPONSE_JOIN_ROOM_PACK;
	
	/*if (room_member_id.find(new_client.id) != room_member_id.end() && room_member_id.count() <= MAX_ROOM_MEMBER_COUNT) {
		room_member_id.insert(new_clinet);
		const wchar_t* result = L"Chat Room Join Success";
		wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), result, _TRUNCATE);
	}
	else {
		const wchar_t* result = L"Chat Room Join Fail";
		wcsncpy_s(result_packet.result, sizeof(result_packet.result) / sizeof(wchar_t), result, _TRUNCATE);
	}*/

	// new_client.send_packet(packet);
}
