// Chatting Game Packet structure.

#define SERVER_PORT 9785
#define BUF_SIZE 1024

#define MAX_BUF 100

enum C2S_PACKET_TYPE {
	SEND_CHAT_PACK = 1,
	REQUEST_CHAT_LOG_PACK
};

#pragma pack (push, 1)

struct C2S_SEND_CHAT_PACK {
	char size;
	char type;
	
	char length;
	char str[MAX_BUF];
};

struct C2S_REQUEST_CHAT_LOG_PACK {
	char size;
	char type;
};

#pragma pack (pop)



enum S2C_PACKET_TYPE {
	SEND_CHAT_LOG_PACK = 1,
	
};

#pragma pack (push, 1)

struct S2C_SEND_CHAT_LOG {
	char size;
	char type;

	char length;
	char str[MAX_BUF];
};

#pragma pack (pop)