#pragma once

class ChatRoomWorkerGrain
{
	// h_iocp_chatroom ���
	std::unordered_map<std::wstring, ChatRoomSession> room_list;
	// ===================
};

