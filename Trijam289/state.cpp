#include "global.h"

bool IsPlayerHome(const BotState &bot) {
	return CheckCollisionRecs({ -1, -1.5f, 2, 3 }, { bot.x - 0.5f, bot.y - 0.5f, 1, 1 });
}

bool IsGooberHome(const Goober &goober) {
	return CheckCollisionRecs({ -1, -1.5f, 2, 3 }, { goober.x - 0.4f, goober.y - 0.4f, 0.8f, 0.8f });
}

void ChooseBrokenTile(GameState &state) {
	state.broken_tile.x = GetRandomValue(-GAME_SIZE, GAME_SIZE);
	state.broken_tile.y = GetRandomValue(-GAME_SIZE, GAME_SIZE);

	if (Dist(state.broken_tile.x, state.broken_tile.y) < 3) {
		ChooseBrokenTile(state);
	}
}

float BotBrokenTileDist(const GameState &state) {
	return Dist(state.broken_tile.x, state.broken_tile.y, state.bot.x, state.bot.y);
}

bool IsBotOnBrokenTile(const GameState &state) {
	return BotBrokenTileDist(state) < 1.f;
}

void PlayExplosion(GameState &state, float x, float y) {
	PlaySound(SND_EXPLOSION);
	state.explosion.rem = EXP_DUR;
	state.explosion.x = x;
	state.explosion.y = y;
}