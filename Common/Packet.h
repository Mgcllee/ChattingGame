// Chatting Game Packet structure.

#define SERVER_PORT 9785
#define BUF_SIZE 1024
#define MAX_MAP_SIZE 1000

#define MAX_BUF 100

enum C2S_PACKET_TYPE {
	LOGIN_PACK = 1,
	REQUEST_JOIN_ROOM_PACK,
	REQUEST_MAKE_ROOM_PACK,
	SEND_CHAT_PACK,
	REQUEST_CHAT_LOG_PACK,
	REQUEST_SHORTEST_PATH,
	REQUEST_ITEM_DATA_PACK,
	LOGOUT_PACK
};

#pragma pack (push, 2)

struct BASIC_PACK {
	short size;
	short type;
};

struct C2S_LOGIN_PACK : BASIC_PACK {
	wchar_t id[MAX_BUF];
	wchar_t pw[MAX_BUF];
};

struct C2S_REQUEST_JOIN_ROOM_PACK : BASIC_PACK {
	wchar_t room_name[MAX_BUF];
};

struct C2S_REQUEST_MAKE_ROOM_PACK : BASIC_PACK {
	wchar_t room_name[MAX_BUF];
};

struct C2S_SEND_CHAT_PACK : BASIC_PACK {
	short length;
	wchar_t str[MAX_BUF];
};

struct C2S_REQUEST_CHAT_LOG_PACK : BASIC_PACK {

};

struct C2S_REQUEST_SHORTCUT_PATH_PACK : BASIC_PACK {
	short start_x, start_y, start_z;
	short goal_x, goal_y, goal_z;
};

struct C2S_REQUEST_ITEM_DATA_PACK : BASIC_PACK {
	wchar_t item_name[MAX_BUF];
};

struct C2S_LOGOUT_PACK : BASIC_PACK {
	wchar_t id[MAX_BUF];
	wchar_t pw[MAX_BUF];
};

#pragma pack (pop)



enum S2C_PACKET_TYPE {
	LOGIN_RESULT_PACK = 1,
	RESPONSE_JOIN_ROOM_PACK,
	SEND_CHAT_LOG_PACK,
	RESPONSE_SHORTEST_PATH,
	RESPONSE_ITEM_DATA_PACK,
	LOGOUT_RESULT_PACK
};

#pragma pack (push, 2)

struct S2C_LOGIN_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF];
};

struct S2C_RESPONSE_JOIN_ROOM_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF];
};

struct S2C_SEND_CHAT_LOG_PACK : BASIC_PACK {
	wchar_t str[MAX_BUF];
};

struct S2C_RESPONSE_ITEM_DATA_PACK : BASIC_PACK {
	wchar_t item_name[MAX_BUF];
};

struct S2C_LOGOUT_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF];
};

#pragma pack (pop)