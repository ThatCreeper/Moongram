#include "global.h"

void RenderMap(const GameState &state, RenderTexture2D map) {
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

	DrawCube({ 0, 0.5f, 0 }, 2, 1, 3, IsPlayerHome(state.bot) ? GREEN : DARKGREEN);

	DrawLine3D({ state.bot.x, 0.5f, state.bot.y }, { state.bot.x + sinf(state.bot.rot), 0.5f, state.bot.y + cosf(state.bot.rot) }, RED);

	DrawSphere({ state.bot.x, 0.5f, state.bot.y }, 0.5f, state.bot.alive ? (IsPlayerHome(state.bot) ? DARKGREEN : GREEN) : GRAY);

	for (int i = 0; i < state.gooberc; i++) {
		const Goober &goober = state.goobers[i];
		// Animate from old to new, kinda
		if (goober.dead) {
			float x = SInterp(goober.o_x, goober.x, goober.anim_time, GOOBER_TRANSITION);
			float y = SInterp(goober.o_y, goober.y, goober.anim_time, GOOBER_TRANSITION);
			float mx = sin(goober.anim_time * 2.3f);
			float my = sin(goober.anim_time * 2.5f + 0.3f);
			float mz = sin(goober.anim_time * 2.1f + 0.2f);

			rlDisableDepthMask();
			rlDisableDepthTest();
			DrawSphere({ x + mx, 0.5f + my, y + mz }, 0.25f, Fade(DARKGRAY, 0.7f));
			rlEnableDepthTest();
			rlEnableDepthMask();
		}
		else {
			DrawSphere({ goober.x, 0.5f, goober.y }, 0.4f, goober.angry_time > 0 ? RED : DARKGRAY);
		}
	}

	if (state.explosion.rem > 0) {
		float ern = state.explosion.rem / EXP_DUR;
		rlDisableDepthMask();
		DrawSphere({ state.explosion.x, 0.5f, state.explosion.y }, 0.4f + (1 - ern), Fade(WHITE, 0.3f * ern));
		DrawSphere({ state.explosion.x, 0.5f, state.explosion.y }, 0.8f, Fade(ORANGE, 0.3f * ern));
		rlEnableDepthMask();
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

void RenderScreen(RenderTexture2D map) {
	ClearBackground(GRAY);

	DrawTextureRec(map.texture, { 0, 0, MAP_WIDTH, -MAP_HEIGHT }, { 25, 25 }, WHITE);

	if (IsKeyDown(KEY_W)) {
		DrawKeybindBar("[W] Forward [A]/[D] Rotate", "");
	}
	else {
		DrawKeybindBar("[W] Forward", "");
	}
}