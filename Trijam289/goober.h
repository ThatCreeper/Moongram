#pragma once

struct GameState;

struct Footprint {
	bool exists = false;
	float x;
	float y;
};

struct Goober {
	float x;
	float y;
	float angry_time = 0;
	float o_x;
	float o_y;
	float anim_time = 0;
	float footprint_timer = 0;
	bool dead = true;
	Footprint footprints[FOOTPRINT_COUNT]{};
};

bool UpdateGoobers(GameState &state);
void SpawnGoober(Goober &goober);