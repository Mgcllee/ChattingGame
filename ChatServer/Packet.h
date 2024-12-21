// Chatting Game Packet structure.


#define BUF_SIZE 1024
#define SERVER_PORT 9785


#pragma pack (1)

struct PACKET {
	char size;
	char content[64];
};

#pragma pack (pop)