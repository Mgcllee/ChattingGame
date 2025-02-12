#pragma once

class ChatRoomWorkerGrain
{
	// h_iocp_chatroom ´ã´ç
	std::unordered_map<std::wstring, ChatRoomSession> room_list;
	// ===================
};

