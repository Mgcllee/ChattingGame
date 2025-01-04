#pragma once

#include "GameSession.h"

int main() {
	std::locale::global(std::locale(""));

	GameSession game_session;
	game_session.run_game_session();
	return 0;
}