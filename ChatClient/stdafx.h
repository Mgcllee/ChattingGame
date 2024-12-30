#pragma once

#define SERVER_ADDR "127.0.0.1"
#define PORT_NUM 9785

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <random>
#include <memory>

#include <SFML/Network.hpp>
#include <SFML/System.hpp>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

using namespace std;

#include "Common/Packet.h"