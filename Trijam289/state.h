#pragma once

struct Goober {
	float x;
	float y;
	float angry_time = 0;
};

struct GameState {
	struct {
		float x = 0;
		float y = 0;
		float rot = -3 * PI / 4;
		bool alive = true;
		float respawn_timer = 0;
		int batteries = 0;
		float life = 1;
		float bat_prog = 0;
		float rep_prog = 0;
	} bot;
	struct {
		int x = 0;
		int y = 0;
	} broken_tile;
	struct {
		bool bat_err = false;
		bool danger = false;
		bool flashing = false;
		bool flash = false;
	} popup;
	struct {
		float x = 0;
		float y = 0;
		float rem = 0;
	} explosion;
	int rep_tiles = 0;
	Goober goobers[GOOBER_COUNT];
	int gooberc = GOOBER_INIT_COUNT;
};

bool IsPlayerHome(const GameState &state);
bool IsGooberHome(const Goober &goober);
void SpawnGoober(Goober &goober);
void ChooseBrokenTile(GameState &state);
float BotBrokenTileDist(const GameState &state);
bool IsBotOnBrokenTile(const GameState &state);
void PlayExplosion(GameState &state, float x, float y);