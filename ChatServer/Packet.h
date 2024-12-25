// Chatting Game Packet structure.


#define BUF_SIZE 1024
#define SERVER_PORT 9785

enum S2C_PACKET_TYPE {

};

enum C2S_PACKET_TYPE {
	SEND_CHAT_PACK = 1,
	REQUEST_CHAT_LOG_PACK
};

#pragma pack (1)

struct C2S_SEND_CHAT_PACK {
	char size;
	char type;
	char content[64];
};

struct C2S_REQUEST_CHAT_LOG_PACK {
	char size;
	char type;
	// 
};

#pragma pack (pop)