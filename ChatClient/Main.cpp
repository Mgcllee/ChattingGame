#pragma once 

#include "Client.h"

vector<wstring> user_id;
vector<wstring> chat_sentences;
vector<Client> clients;

void read_files();

int main() {
	read_files();

	char user_type;
	while (true) {
		printf("[유저 모드]: 유저가 직접 채팅을 사용하기 위해 접속\n");
		printf("[기본 모드]: %d 개수의 더미 클라이언트가 접속\n", MAX_CLIENT);
		printf("유저 모드로 접속하시겠습니까?(y/n): ");
		cin >> user_type;
		if (user_type == 'y' || user_type == 'n'
			|| user_type == 'Y' || user_type == 'N')
			continue;
		else printf("잘못된 입력입니다. y 혹은 n 중 하나만 입력해주세요.")
	}

	if (user_type == 'N' || user_type == 'n') {
		for (int i = 0; i < MAX_CLIENT; ++i) {
			clients.emplace_back();
			clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
		}
		printf("Complete all client connect to server and login success\n");

		while (true) {
			sf::sleep(sf::seconds(1.f));
			for (int i = 0; i < MAX_CLIENT; ++i) {
				clients[i].communicate_server(i);
			}
		}

		for (int i = 0; i < MAX_CLIENT; ++i) {
			clients[i].disconnect_to_server();
		}
	}
	else if (user_type == 'Y' || user_type == 'y') {
		clients.emplace_back();
		clients[0].connect_to_server(SERVER_ADDR, PORT_NUM, 0);

		C2S_REQUEST_JOIN_ROOM_PACK packet;
		packet.size = sizeof(pakcet);
		packet.type = C2S_PACKET_TYPE::REQUEST_JOIN_ROOM_PACK;
		
		printf("입장할 방의 이름을 입력해주세요: ");
		wcin >> packet.room_name;

		clients[0].send_packet(packet);


	}
	return 0;
}


void read_files() {
	locale::global(locale(""));

	wifstream readFile("mixed_sentences.txt");
	readFile.imbue(locale(readFile.getloc(), new codecvt_utf8<wchar_t>));
	if (readFile.is_open()) {
		wstring sentences;

		for (int i = 0; i < MAX_SENTENCE; ++i) {
			getline(readFile, sentences);
			chat_sentences.push_back(sentences);
		}
		readFile.close();
	}
	else {
		printf("Can't read mixed_sentences file...\n");
		return;
	}

	readFile.open("valid_game_names_with_korean_words.txt");
	readFile.imbue(locale(readFile.getloc(), new codecvt_utf8<wchar_t>));
	if (readFile.is_open()) {
		wstring sentences;

		for (int i = 0; i < MAX_SENTENCE; ++i) {
			getline(readFile, sentences);
			user_id.push_back(sentences);
		}
		readFile.close();
	}
	else {
		printf("Can't read valid_game_names_with_korean_words file...\n");
		return;
	}
}