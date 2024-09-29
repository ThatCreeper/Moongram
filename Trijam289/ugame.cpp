#include "global.h"

bool UpdatedRunGame() {
	int fadein = 0;
	GameState state{};
	RenderTexture2D map = LoadRenderTexture(MAP_WIDTH, MAP_HEIGHT);

	for (Goober &goober : state.goobers) {
		SpawnGoober(goober);
	}
	ChooseBrokenTile(state);

	PlaySound(SND_START);

	DoFadeInAnimation();

	while (!WindowShouldClose()) {
		// Input
		MoveBotAndDrainLife(state.bot);

		// Melee doesn't work :(
		//if (state.bot_alive && state.bat > 0 && IsKeyPressed(KEY_ENTER)) {
		//	state.bat--;
		//	PlaySound(SND_FIRE);
		//	float dist = 8.f;
		//	Goober *goob = nullptr;
		//	for (int i = 0; i < gooberc; i++) {
		//		Goober &goober = goobers[i];
		//		RayCollision c = GetRayCollisionSphere({ {state.bot_x, 0, state.bot_y}, {sinf(state.bot_rot) * 8.f, cosf(state.bot_rot) * 8.f} }, { goober.x, 0, goober.y }, 2.f);
		//		state.expr = EXP_DUR;
		//		state.expx = state.bot_x + sinf(state.bot_rot) * 3;
		//		state.expy = state.bot_y + cosf(state.bot_rot) * 3;
		//		if (c.distance < dist) {
		//			dist = c.distance;
		//			goob = &goober;
		//		}
		//	}
		//	if (goob) {
		//		PlaySound(SND_EXPLOSION);
		//		state.expr = EXP_DUR;
		//		state.expx = goob->x;
		//		state.expy = goob->y;
		//		SpawnGoober(*goob);
		//	}
		//}
		state.explosion.rem -= GetFrameTime();

		/*if (IsKeyPressed(KEY_BACKSPACE)) {
			state.bot_alive = false;
		}*/

		// collision
		/*if (state.bot_x > 0 && state.bot_x < 2 && state.bot_y > -1.5f && state.bot_y < 1.5f) {
			state.bot_x = 2;
		}*/

		if (state.bot.alive && state.bot.life <= 0) {
			state.bot.alive = false;
			PlaySound(SND_DIE);
		}

		if (UpdateGoobers(state)) {
			if (DisplayDeath(state)) {
				goto RESTART;
			}
			else {
				goto END;
			}
		}

		UpdateTimers(state);

		state.bot.life = Clamp(state.bot.life, 0, 1);

		RenderMap(state, map);
		RenderScreen(fadein, map);
	}

END:
	if (state.rep_tiles > globstate.hscore_up)
		globstate.hscore_up = state.rep_tiles;
	SaveGlobState();

	return false;
RESTART:
	if (state.rep_tiles > globstate.hscore_up)
		globstate.hscore_up = state.rep_tiles;
	SaveGlobState();

	UnloadRenderTexture(map);

	return true;
}