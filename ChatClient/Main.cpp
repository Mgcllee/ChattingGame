#pragma once 

#include "Client.h"
#include <thread>

vector<wstring> user_id;
vector<wstring> chat_sentences;
vector<Client> clients;

void read_files();
void run_clients_communication(int start, int end);


int main() {
	read_files();

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients.emplace_back();
		clients[i].connect_to_server(SERVER_ADDR, PORT_NUM, i);
	}
	wprintf(L"Complete all client connect to server and login success\n");

	run_clients_communication(0, MAX_CLIENT);

	vector<thread> threads;
	/*for (int i = 0; i < 2; ++i) {
		threads.emplace_back(run_clients_communication, MAX_CLIENT / 2 * i, MAX_CLIENT / 2 * (i + 1));
	}*/

	threads.emplace_back(run_clients_communication, 0, 5);
	threads.emplace_back(run_clients_communication, 5, 10);

	for (auto& th : threads) {
		th.join();
	}

	for (int i = 0; i < MAX_CLIENT; ++i) {
		clients[i].disconnect_to_server();
	}


	return 0;
}

void run_clients_communication(int start, int end) {
	while (true) {
		for (int i = start; i < end; ++i) {
			clients[i].communicate_server(i);
		}
	}
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
		wprintf(L"Can't read mixed_sentences file...\n");
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
		wprintf(L"Can't read valid_game_names_with_korean_words file...\n");
		return;
	}
}