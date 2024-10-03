#pragma once

struct GameState;

struct Goober {
	float x;
	float y;
	float angry_time = 0;
	float o_x;
	float o_y;
	float anim_time = 0;
	bool dead = false;
};

bool UpdateGoobers(GameState &state);
void SpawnGoober(Goober &goober);