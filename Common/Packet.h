// Chatting Game Packet structure.

#define SERVER_PORT 9785
#define BUF_SIZE 1024

#define MAX_BUF 100

enum C2S_PACKET_TYPE {
	LOGIN_PACK = 1,
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
	wchar_t id[MAX_BUF];
	wchar_t pw[MAX_BUF];
};

struct C2S_SEND_CHAT_PACK : BASIC_PACK {
	short length;
	wchar_t str[MAX_BUF];
};

struct C2S_REQUEST_SHORTCUT_PATH : BASIC_PACK {

};

struct C2S_REQUEST_CHAT_LOG_PACK : BASIC_PACK {
	
};

struct C2S_LOGOUT_PACK : BASIC_PACK {
	wchar_t id[MAX_BUF];
	wchar_t pw[MAX_BUF];
};

#pragma pack (pop)



enum S2C_PACKET_TYPE {
	LOGIN_RESULT_PACK = 1,
	SEND_CHAT_LOG_PACK,
	LOGOUT_RESULT_PACK
};

#pragma pack (push, 2)

struct S2C_LOGIN_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF];
};

struct S2C_SEND_CHAT_LOG_PACK : BASIC_PACK {
	wchar_t str[MAX_BUF];
};

struct S2C_LOGOUT_RESULT_PACK : BASIC_PACK {
	wchar_t result[MAX_BUF];
};

#pragma pack (pop)