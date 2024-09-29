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
		if (state.bot.alive && IsKeyDown(KEY_W)) {
			if (state.bot.rep_prog > 0) {
				state.bot.rep_prog = 0;
				StopSound(SND_PROGRESS1);
			}
			float dec = 0.0005f;
			if (IsKeyDown(KEY_A)) {
				state.bot.rot += 2.f * GetFrameTime();
				dec = 0.0007f;
			}
			if (IsKeyDown(KEY_D)) {
				state.bot.rot -= 2.f * GetFrameTime();
				dec = 0.0007f;
			}
			state.bot.x += sinf(state.bot.rot) * GetFrameTime() * 5.f;
			state.bot.y += cosf(state.bot.rot) * GetFrameTime() * 5.f;
			state.bot.life -= dec;
		}

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
		if (state.bot.x < -GAME_SIZE)
			state.bot.x = -GAME_SIZE;
		if (state.bot.y < -GAME_SIZE)
			state.bot.y = -GAME_SIZE;
		if (state.bot.x > GAME_SIZE)
			state.bot.x = GAME_SIZE;
		if (state.bot.y > GAME_SIZE)
			state.bot.y = GAME_SIZE;

		state.bot.life -= 0.0003f;

		if (state.bot.alive && state.bot.life <= 0) {
			state.bot.alive = false;
			PlaySound(SND_DIE);
		}

		bool wasdanger = state.popup.danger;
		state.popup.danger = false;
		state.popup.flash = !state.popup.flash;
		state.popup.flashing = false;
		for (int i = 0; i < state.gooberc; i++) {
			Goober &goober = state.goobers[i];
			goober.angry_time -= GetFrameTime();
			float cdist = goober.angry_time > 0 ? 6 : 4;
			if (Dist(state.bot.x, state.bot.y, goober.x, goober.y) < cdist) {
				if (goober.angry_time <= 0) {
					PlaySound(SND_DETECTION);
				}
				goober.angry_time = 3;
			}
			float pxd = state.bot.x - goober.x;
			float pyd = state.bot.y - goober.y;
			float pd = sqrtf(pxd * pxd + pyd * pyd);
			float hd = sqrtf(goober.x * goober.x + goober.y * goober.y);
			if (goober.angry_time < 0 || !state.bot.alive || IsPlayerHome(state) || pd > 7 || (hd * 2 < pd && hd > 3.f)) {
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
				state.bot.alive = false;
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
			if (IsGooberHome(goober)) {
				if (DisplayDeath(state)) {
					goto RESTART;
				}
				else {
					goto END;
				}
			}
		}
		if (!wasdanger && state.popup.danger)
			PlaySound(SND_MENU);

		state.popup.bat_err = false;
		if (state.bot.alive) {
			if (IsPlayerHome(state)) {
				state.bot.bat_prog += GetFrameTime();
				state.bot.life += 0.0008f;
				PlaySound(SND_PROGRESS);
			}
			else {
				state.bot.bat_prog = 0;
				StopSound(SND_PROGRESS);
			}

			if (state.bot.bat_prog >= BAT_DUR) {
				state.bot.bat_prog = 0;
				state.bot.batteries++;
				StopSound(SND_PROGRESS);
				PlaySound(SND_REPAIR);
			}

			if (IsBotOnBrokenTile(state)) {
				if (state.bot.batteries > 0) {
					state.bot.rep_prog += GetFrameTime();
					PlaySound(SND_PROGRESS1);
				}
				else {
					state.popup.bat_err = true;
				}
			}
			else {
				state.bot.rep_prog = 0;
				StopSound(SND_PROGRESS1);
			}

			if (state.bot.rep_prog >= REP_DUR) {
				state.bot.rep_prog = 0;
				state.bot.batteries--;
				state.rep_tiles++;
				ChooseBrokenTile(state);

				if (state.rep_tiles % 5 == 0) {
					PlaySound(SND_COMBO);
					state.gooberc = Clamp(state.gooberc + 1, GOOBER_INIT_COUNT, GOOBER_COUNT);
				}
				else {
					PlaySound(SND_REPAIR);
				}
			}
		}
		else {
			state.bot.respawn_timer += GetFrameTime();
			state.bot.bat_prog = 0;

			if (state.bot.respawn_timer >= 3.f) {
				state.bot.x = 0;
				state.bot.y = 0;
				state.bot.alive = true;
				state.bot.respawn_timer = 0;
				state.bot.batteries = 0;
				state.bot.life = 1;
			}
		}

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