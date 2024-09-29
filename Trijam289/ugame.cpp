#include "global.h"

bool UpdatedRunGame() {
	int fadein = 0;
	GameState state;
	RenderTexture2D map = LoadRenderTexture(MAP_WIDTH, MAP_HEIGHT);
	Goober goobers[GOOBER_COUNT];
	int gooberc = GOOBER_INIT_COUNT;

	for (Goober &goober : goobers) {
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
		for (int i = 0; i < gooberc; i++) {
			Goober &goober = goobers[i];
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
					gooberc++;
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

		{
			BeginTextureMode(map);

			ClearBackground(DARKBLUE);

			Camera3D cam{ 0 };
			cam.fovy = 10;
			cam.projection = CAMERA_ORTHOGRAPHIC;
			cam.up.y = 1;
			cam.position = { 5 + state.bot.x, 5, 5 + state.bot.y };
			cam.target = { state.bot.x, 0, state.bot.y };

			BeginMode3D(cam);

			DrawGrid(4 * GAME_SIZE, 0.5F);

			DrawPlane({ (float)state.broken_tile.x, 0, (float)state.broken_tile.y }, { 1, 1 }, state.bot.batteries > 0 ? YELLOW : BROWN);

			DrawCube({ 0, 0.5f, 0 }, 2, 1, 3, IsPlayerHome(state) ? GREEN : DARKGREEN);

			DrawLine3D({ state.bot.x, 0.5f, state.bot.y }, { state.bot.x + sinf(state.bot.rot), 0.5f, state.bot.y + cosf(state.bot.rot) }, RED);

			DrawSphere({ state.bot.x, 0.5f, state.bot.y }, 0.5f, state.bot.alive ? GREEN : GRAY);

			for (int i = 0; i < gooberc; i++) {
				Goober &goober = goobers[i];
				DrawSphere({ goober.x, 0.5f, goober.y }, 0.4f, goober.angry_time > 0 ? RED : DARKGRAY);
			}

			if (state.explosion.rem > 0) {
				DrawSphere({ state.explosion.x, 0.5f, state.explosion.y }, 0.6f, Fade(ORANGE, 0.3f * (state.explosion.rem / EXP_DUR)));
			}

			EndMode3D();

			if (state.popup.danger && !(state.popup.flashing && state.popup.flash)) {
				if (state.popup.flashing)
					PlaySound(SND_MENU);
				Vector2 hPos = GetWorldToScreen({ 0, 0.5f, 0 }, cam);

				hPos.x = Clamp(hPos.x, 15, MAP_WIDTH - 15 - 15);
				hPos.y = Clamp(hPos.y, 15, MAP_HEIGHT - 15 - 15);

				DrawRectangle(hPos.x, hPos.y, 15, 15, RED);
			}
			if (state.bot.batteries > 0) {
				Vector2 btPos = GetWorldToScreen({ (float)state.broken_tile.x, 0, (float)state.broken_tile.y }, cam);

				btPos.x = Clamp(btPos.x, 15, MAP_WIDTH - 15 - 15);
				btPos.y = Clamp(btPos.y, 15, MAP_HEIGHT - 15 - 15);

				DrawRectangle(btPos.x, btPos.y, 15, 15, YELLOW);
			}

			DrawText(TextFormat("SCORE %d", state.rep_tiles), 15, 15, 20, WHITE);
			DrawText(TextFormat("PART %d", state.bot.batteries), 15, 40, 20, WHITE);
			DrawText(TextFormat("BAT %d", (int)ceil(state.bot.life * 100)), 15, 65, 20, WHITE);

			if (state.bot.bat_prog > 0) {
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200, 20, BLACK);
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200 * (state.bot.bat_prog / BAT_DUR), 20, WHITE);
			}
			if (state.bot.rep_prog > 0) {
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200, 20, BLACK);
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200 * (state.bot.rep_prog / REP_DUR), 20, YELLOW);
			}
			if (state.popup.bat_err) {
				const char *t = "No Substitute Part!";
				int w = MeasureText(t, 20);
				DrawText(t, (MAP_WIDTH - w) / 2, MAP_HEIGHT / 2 - 50, 20, RED);
			}

			if (!state.bot.alive) {
				DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.4f));
				const char *t = "Summoning a new robot...";
				int w = MeasureText(t, 50);
				DrawText(t, (MAP_WIDTH - w) / 2, (MAP_HEIGHT) / 2 - 50, 50, RED);
				t = TextFormat("Please wait %0.1f seconds", 3.f - state.bot.respawn_timer);
				w = MeasureText(t, 50);
				DrawText(t, (MAP_WIDTH - w) / 2, (MAP_HEIGHT) / 2, 50, RED);
			}

			EndTextureMode();

		}

		BeginDrawing();

		ClearBackground(GRAY);

		DrawTextureRec(map.texture, { 0, 0, MAP_WIDTH, -MAP_HEIGHT }, { 25, 25 }, WHITE);

		if (IsKeyDown(KEY_W)) {
			DrawKeybindBar("[W] Forward [A]/[D] Rotate", "");
		}
		else {
			DrawKeybindBar("[W] Forward", "");
		}

		DoFadeOutAnimation(fadein);

		EndDrawing();
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