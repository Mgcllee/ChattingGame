#pragma once

#define SERVER_ADDR "127.0.0.1"
#define PORT_NUM 9785

#define MAX_CLIENT 3 // 50'000
#define MAX_SENTENCE 100'000

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <random>
#include <memory>
#include <fstream>
#include <locale>
#include <codecvt>
#include <Windows.h>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#include "Common/Packet.h"

extern vector<wstring> user_id;
extern vector<wstring> chat_sentences;

enum JOB_TYPE {
	USER_LOGIN = 1,
	SEND_CHAT,
	RECV_CHAT,
	REQUEST_CHAT_LOG,
	USER_LOGOUT	// JOB_TYPE 개수 확인용으로 반드시 마지막에 위치.
};