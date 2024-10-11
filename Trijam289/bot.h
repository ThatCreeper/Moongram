#pragma once

struct GameState;

enum DeathCause {
	DC_BUG, // This is never used.
	DC_MAULED,
	DC_BATTERY
};

struct BotState {
	float x = 0;
	float y = 0;
	float rot = -3 * PI / 4;
	bool alive = true;
	float respawn_timer = 0;
	int batteries = 0;
	float life = 1;
	float bat_prog = 0;
	float rep_prog = 0;
	DeathCause death_cause = DC_BUG;
};

void KillBot(BotState &bot, DeathCause cause);
void UpdateBot(GameState &state);