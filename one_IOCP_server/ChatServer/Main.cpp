#pragma once

#include "Public/Silo/GameServerSilo.h"

int main() {
	std::locale::global(std::locale(""));

	GameServerSilo game_session;
	game_session.run_game_logic_grains();
	return 0;
}