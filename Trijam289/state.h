#pragma once

struct GameState {
	BotState bot;
	struct BrokenTileState {
		int x = 0;
		int y = 0;
	} broken_tile;
	struct PopupState {
		bool bat_err = false;
		bool danger = false;
		bool flashing = false;
		bool flash = false;
	} popup;
	struct ExplosionState {
		float x = 0;
		float y = 0;
		float rem = 0;
	} explosion;
	int rep_tiles = 0;
	Goober goobers[GOOBER_COUNT]{};
	int gooberc = GOOBER_INIT_COUNT;
};

struct GameFlags {
	bool modified_movement = false;
};
extern GameFlags gameflags;

bool IsPlayerHome(const BotState &bot);
bool IsGooberHome(const Goober &goober);
void ChooseBrokenTile(GameState &state);
float BotBrokenTileDist(const GameState &state);
bool IsBotOnBrokenTile(const GameState &state);
void PlayExplosion(GameState &state, float x, float y);