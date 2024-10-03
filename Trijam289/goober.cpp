#include "global.h"

static void ProcessGooberAnger(GameState &state, Goober &goober) {
	goober.angry_time -= GetFrameTime();
	float cdist = goober.angry_time > 0 ? 6 : 4;
	if (Dist(state.bot.x, state.bot.y, goober.x, goober.y) < cdist) {
		if (goober.angry_time <= 0) {
			PlaySound(SND_DETECTION);
		}
		goober.angry_time = 3;
	}
}

static bool DoesGooberPlayerVisionIntersectHomeBase(GameState &state, Goober &goober) {
	float pxd = state.bot.x - goober.x;
	float pyd = state.bot.y - goober.y;
	float pd = sqrtf(pxd * pxd + pyd * pyd);
	// { -1, -1.5f, 2, 3 }

	RayCollision collision = GetRayCollisionBox({ { goober.x, 0, goober.y }, { pxd, 0, pyd } }, { { -1, -1, -1.5f }, { 1, 1, 1.5f } });
	return collision.hit && collision.distance < pd;
}

static void ProcessGooberMovement(GameState &state, Goober &goober) {
	float pxd = state.bot.x - goober.x;
	float pyd = state.bot.y - goober.y;
	float pd = sqrtf(pxd * pxd + pyd * pyd);
	float hd = sqrtf(goober.x * goober.x + goober.y * goober.y);
	if (goober.angry_time < 0 || !state.bot.alive || IsPlayerHome(state.bot) || DoesGooberPlayerVisionIntersectHomeBase(state, goober) || pd > 7 || (hd * 2 < pd && hd > 3.f)) {
		goober.x -= (goober.x / hd) * GetFrameTime() * 0.3f;
		goober.y -= (goober.y / hd) * GetFrameTime() * 0.3f;
	}
	else if (goober.angry_time > 0) {
		pxd /= pd;
		pyd /= pd;
		goober.x += pxd * GetFrameTime() * 1.3f;
		goober.y += pyd * GetFrameTime() * 1.3f;
	}

	if (pd < 0.7f) {
		KillBot(state.bot, DC_MAULED);
		PlayExplosion(state, state.bot.x, state.bot.y);
		SpawnGoober(goober);
		PlaySound(SND_DIE);
	}
	if (hd < 6) {
		state.popup.danger = true;
	}
	if (hd < 4) {
		state.popup.flashing = true;
	}
}

void SpawnGoober(Goober &goober) {
	int srad = GAME_SIZE * 1.25;

	goober.dead = true;
	goober.o_x = goober.x;
	goober.o_y = goober.y;
	goober.anim_time = 0;
	goober.x = GetRandomValue(-srad, srad);
	goober.y = GetRandomValue(-srad, srad);
	goober.angry_time = 0;

	if (goober.x * goober.x + goober.y * goober.y < 15) {
		SpawnGoober(goober);
	}
}

bool UpdateGoobers(GameState &state) {
	bool wasdanger = state.popup.danger;
	state.popup.danger = false;
	state.popup.flash = !state.popup.flash;
	state.popup.flashing = false;

	for (int i = 0; i < state.gooberc; i++) {
		Goober &goober = state.goobers[i];

		if (goober.dead) {
			goober.anim_time += GetFrameTime();

			if (goober.anim_time >= GOOBER_TRANSITION) {
				goober.dead = false;
				goober.anim_time = 0;
			}

			continue;
		}

		ProcessGooberAnger(state, goober);

		ProcessGooberMovement(state, goober);
		
		if (IsGooberHome(goober)) {
			return true;
		}
	}
	if (!wasdanger && state.popup.danger)
		PlaySound(SND_MENU);
	return false;
}