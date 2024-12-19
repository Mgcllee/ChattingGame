#pragma once 

#include "stdafx.h"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"

#include <vector>
#include <cstring>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

struct PACKET {
	char size;
	string content;
};

int main() {
	int clinet_num = 10;
	vector<unique_ptr<sf::TcpSocket>> sockets;
	for (int i = 0; i < clinet_num; ++i) {
		sockets.push_back(make_unique<sf::TcpSocket>());
	}

	for (int i = 0; i < clinet_num; ++i) {

		sf::sleep(sf::seconds(3));

		int PORT_NUM = 9785;
		sf::Socket::Status status = sockets[i]->connect("127.0.0.1", PORT_NUM);
		sockets[i]->setBlocking(false);

		if (status != sf::Socket::Done) {
			printf("서버와 연결할 수 없습니다.\n");
			return 0;
		}

		printf("Connect Server!\n");

		
		PACKET packet;
		packet.content = "Hello World!";
		packet.size = sizeof(packet);

		size_t sent;
		if (sf::Socket::Done != sockets[i]->send(reinterpret_cast<const void*>(&packet), (size_t)packet.size, sent)) {
			printf("클라이언트 송신 오류\n");
		}
	}
	
	for (int i = 0; i < clinet_num; ++i) {
		sockets[i]->disconnect();
	}

	return 0;
}