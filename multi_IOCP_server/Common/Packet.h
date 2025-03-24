// Chatting Game Packet structure.

#define SERVER_PORT 9785
#define MAX_PACKET_SIZE 1024
#define MAX_BUF_SIZE 200
#define MAX_ROOM_MEMBER_COUNT 100

enum C2S_PACKET_TYPE {
	LOGIN_PACK = 1,
	REQUEST_JOIN_ROOM_PACK,
	SEND_CHAT_PACK,
	REQUEST_CHAT_LOG_PACK,
	LOGOUT_PACK
};

#pragma pack (push, 2)

struct BASIC_PACK {
	short size;
	short type;
};

struct C2S_LOGIN_PACK : BASIC_PACK {
	wchar_t id[MAX_BUF_SIZE];
	wchar_t pw[MAX_BUF_SIZE];
};

struct C2S_REQUEST_JOIN_ROOM_PACK : BASIC_PACK {
	wchar_t room_name[MAX_BUF_SIZE];
};

struct C2S_REQUEST_MAKE_ROOM_PACK : BASIC_PACK {
	wchar_t room_name[MAX_BUF_SIZE];
	short maximum_member_count;
};

struct C2S_SEND_CHAT_PACK : BASIC_PACK {
	short length;			
	wchar_t str[MAX_BUF_SIZE];
};

struct C2S_REQUEST_CHAT_LOG_PACK : BASIC_PACK {
	wchar_t start_time[MAX_BUF_SIZE];
	wchar_t end_time[MAX_BUF_SIZE];
};

struct C2S_LOGOUT_PACK : BASIC_PACK {
	wchar_t id[MAX_BUF_SIZE];
	wchar_t pw[MAX_BUF_SIZE];
};

#pragma pack (pop)



enum S2C_PACKET_TYPE {
	LOGIN_RESULT_PACK = 1,
	RESPONSE_JOIN_ROOM_PACK,
	RESPONSE_MAKE_ROOM_PACK,
	RESPONSE_CHAT_LOG_PACK,
	RESPONSE_EXIST_CLIENTS,
	LOGOUT_RESULT_PACK
};

#pragma pack (push, 2)

struct S2C_LOGIN_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF_SIZE];
};

struct S2C_RESPONSE_JOIN_ROOM_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF_SIZE];
};

struct S2C_RESPONSE_MAKE_ROOM_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF_SIZE];
};

struct S2C_SEND_CHAT_LOG_PACK : BASIC_PACK {
	wchar_t str[MAX_BUF_SIZE];
};

struct S2C_LOGOUT_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF_SIZE];
};

#pragma pack (pop)