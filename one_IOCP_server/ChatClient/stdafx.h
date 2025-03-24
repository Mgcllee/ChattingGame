#pragma once

#define SERVER_ADDR "127.0.0.1"
#define PORT_NUM 9785

#define MAX_CLIENT 10
#define MAX_SENTENCE 100'000

#define MAX_THREAD 1

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <random>
#include <fstream>
#include <locale>
#include <codecvt>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

#include "Common/Packet.h"

extern vector<wstring> user_id;
extern vector<wstring> chat_sentences;

enum JOB_TYPE {
	USER_LOGIN = 1,
	SEND_CHAT,
	USER_LOGOUT
};