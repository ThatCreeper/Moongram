#include "global.h"

// WHATEVER YOU DO, DO NOT ADD CLASSES PLEASE FOR THE LOVE OF GOD. OR, IF YOU DO, THINK ABOUT IT. THINK "DO I NEED THIS". THINK THAT AND THEN SAY /NO/!

#ifdef _DEBUG
// #define FORCE_EDITION 0
#endif

void DrawKeybindBar(const char *left, const char *right, bool bg = true) {
	if (bg)
		DrawRectangle(0, 600 - 30, 800, 30, Fade(BLACK, 0.7f));
	DrawLine(0, 600 - 31, 800, 600 - 31, WHITE); // I dislike the number "31" here, but it is correct. Sad.
	DrawText(left, 10, 600 - 25, 20, WHITE);
	int rlen = MeasureText(right, 20);
	DrawText(right, 800 - 10 - rlen, 600 - 25, 20, WHITE);
}

void ProcessFlags(int sel) {
	// nothing yet
}

bool PickFlags() {
#ifdef FORCE_EDITION
	ProcessFlags(FORCE_EDITION);
	return true;
#endif

	int sel = 0;
	Color overflow = BLACK;
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_UP)) {
			sel--;
			PlaySound(SND_MENU);
			overflow = BLACK;
		} if (IsKeyPressed(KEY_DOWN)) {
			sel++;
			PlaySound(SND_MENU);
			overflow = BLACK;
		}

		if (sel > 0) {
			sel = 0;
			overflow = BLUE;
		}
		else if (sel < 0) {
			sel = 0;
			overflow = ORANGE;
		}

		if (IsKeyPressed(KEY_ENTER)) {
			ProcessFlags(sel);
			return true;
		}

		BeginDrawing();

		ClearBackground(BLACK);

		DrawText("Edition of Moongram:", 15, 15, 20, WHITE);

		DrawLine(15, 45, 395, 45, overflow);
		DrawLine(15, 85, 395, 85, overflow);
		//DrawLine(15, 165, 395, 165, overflow);

		if (sel == 0) DrawRectangle(15, 50, 380, 30, DARKGRAY);
		if (sel == 0) DrawRectangleLines(15, 50, 380, 30, WHITE);
		if (sel == 0) DrawText("Version of the game made\n\nduring the three hours\n\nof the Trijam.", 460, 50, 20, WHITE);
		DrawText("3-Hour Edition", 25, 55, 20, WHITE);

		//if (sel == 1) DrawRectangle(15, 90, 380, 30, DARKGRAY);
		//if (sel == 1) DrawRectangleLines(15, 90, 380, 30, WHITE);
		//if (sel == 1) DrawText("Version of blah\n\nduring the three hours\n\nof the Trijam.", 460, 50, 20, WHITE);
		//DrawText("blah", 25, 95, 20, WHITE);

		//if (sel == 2) DrawRectangle(15, 130, 380, 30, DARKGRAY);
		//if (sel == 2) DrawRectangleLines(15, 130, 380, 30, WHITE);
		//if (sel == 2) DrawText("wal\n\n\n\n\nnut", 460, 50, 20, WHITE);
		//DrawText("ijd wadawd", 25, 135, 20, WHITE);

		DrawKeybindBar("[Up] [Down]", "[Enter] Select");

		EndDrawing();
	}
	return false;
}

void DoFadeInAnimation() {
	int top = 0;

	while ((top += 80) <= 800) {
		float t = ((float)top) / 800.f;
		t *= t;
		t *= 800;

		BeginDrawing();

		DrawRectangle(0, 0, t, 600, BLUE);

		EndDrawing();
	}
}

void DoFadeOutAnimation(int &top) {
	if (top < 800) {
		top += 60;
		float t = ((float)top) / 800.f;
		t *= t;
		t *= 800;

		DrawRectangle(t, 0, 800, 600, BLUE);
	}
}

#define MAP_WIDTH (800 - 50)
#define MAP_HEIGHT (600 - 50 - 30)
#define GAME_SIZE 20

#define BAT_DUR 2
#define REP_DUR 3
#define EXP_DUR 1.5f

struct GlobState {
	int hscore = 0;
} globstate;

SERIALIZER(GlobState) {
	ADD(SR_INIT, hscore);
} SERIALIZER_END

void SaveGlobState() {
	R r = RWrite("save.dat");
	if (r.file) {
		SER_REV(r);
		SERIALIZE(r, globstate);
	}
	RClose(r);
}

struct GameState {
	float bot_x = 0;
	float bot_y = 0;
	float bot_rot = -3 * PI / 4;
	bool bot_alive = true;
	float bot_respawn_timer = 0;
	int bat = 0;
	float life = 1;
	float bat_prog = 0;
	int btx = 0;
	int bty = 0;
	float rep_prog = 0;
	int rep_tiles = 0;
	bool bat_err = false;
	bool danger = false;
	bool flashing = false;
	bool fstate = false;
	float expx = 0;
	float expy = 0;
	float expr = 0;
};

bool PlayerHome(const GameState &state) {
	return CheckCollisionRecs({ -1, -1.5f, 2, 3 }, { state.bot_x - 0.5f, state.bot_y - 0.5f, 1, 1 });
}

struct Goober {
	float x;
	float y;
	float angrytime = 0;
};

bool GooberHomeHome(const Goober &goober) {
	return CheckCollisionRecs({ -1, -1.5f, 2, 3 }, { goober.x - 0.4f, goober.y - 0.4f, 0.8f, 0.8f });
}

float Dist(float ax, float ay, float bx, float by) {
	float xd = ax - bx;
	float yd = ay - by;
	return sqrtf(xd * xd + yd * yd);
}

void SpawnGoober(Goober &goober) {
	int srad = GAME_SIZE * 1.25;

	goober.x = GetRandomValue(-srad, srad);
	goober.y = GetRandomValue(-srad, srad);
	goober.angrytime = 0;

	if (goober.x * goober.x + goober.y * goober.y < 4) {
		SpawnGoober(goober);
	}
}

void ChooseBrokenTile(GameState &state) {
	state.btx = GetRandomValue(-GAME_SIZE, GAME_SIZE);
	state.bty = GetRandomValue(-GAME_SIZE, GAME_SIZE);

	if (state.btx * state.btx + state.bty * state.bty < 3) {
		ChooseBrokenTile(state);
	}
}

float BrokeTileDist(const GameState &state) {
	return Dist(state.btx, state.bty, state.bot_x, state.bot_y);
}

bool OnBrokeTile(const GameState &state) {
	return BrokeTileDist(state) < 1.f;
}

#define GOOBER_COUNT 8
#define GOOBER_INIT_COUNT 3

bool DisplayDeath(const GameState &state) {
	bool f = true;

	PlaySound(SND_GAME_OVER);

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_ENTER)) {
			return true;
		}

		BeginDrawing();

		//if (f) {
		//	f = false;
			DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.1f));
		//}

		const char *s = "You lost.";
		int m = MeasureText(s, 30);
		DrawText(s, (800 - m) / 2, (600 - 60) / 2, 30, WHITE);
		s = TextFormat("Before that, you repaired %d tiles!", state.rep_tiles);
		m = MeasureText(s, 30);
		DrawText(s, (800 - m) / 2, (600) / 2, 30, WHITE);
		if (state.rep_tiles > globstate.hscore) {
			s = "(That's a new high score!)";
			m = MeasureText(s, 30);
			DrawText(s, (800 - m) / 2, (600 + 60) / 2, 30, WHITE);
		}

		DrawKeybindBar("", "[Enter] Restart", false);

		EndDrawing();
	}

	return false;
}

float Clamp(float x, float min, float max) {
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

bool RunGame() {
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
		if (state.bot_alive && IsKeyDown(KEY_W)) {
			if (state.rep_prog > 0) {
				state.rep_prog = 0;
				StopSound(SND_PROGRESS1);
			}
			float dec = 0.0005f;
			if (IsKeyDown(KEY_A)) {
				state.bot_rot += 2.f * GetFrameTime();
				dec = 0.0007f;
			}
			if (IsKeyDown(KEY_D)) {
				state.bot_rot -= 2.f * GetFrameTime();
				dec = 0.0007f;
			}
			state.bot_x += sinf(state.bot_rot) * GetFrameTime() * 5.f;
			state.bot_y += cosf(state.bot_rot) * GetFrameTime() * 5.f;
			state.life -= dec;
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
		state.expr -= GetFrameTime();

		/*if (IsKeyPressed(KEY_BACKSPACE)) {
			state.bot_alive = false;
		}*/

		// collision
		/*if (state.bot_x > 0 && state.bot_x < 2 && state.bot_y > -1.5f && state.bot_y < 1.5f) {
			state.bot_x = 2;
		}*/
		if (state.bot_x < -GAME_SIZE)
			state.bot_x = -GAME_SIZE;
		if (state.bot_y < -GAME_SIZE)
			state.bot_y = -GAME_SIZE;
		if (state.bot_x > GAME_SIZE)
			state.bot_x = GAME_SIZE;
		if (state.bot_y > GAME_SIZE)
			state.bot_y = GAME_SIZE;

		state.life -= 0.0003f;

		if (state.bot_alive && state.life <= 0) {
			state.bot_alive = false;
			PlaySound(SND_DIE);
		}
		
		bool wasdanger = state.danger;
		state.danger = false;
		state.fstate = !state.fstate;
		state.flashing = false;
		for (int i = 0; i < gooberc; i++) {
			Goober &goober = goobers[i];
			goober.angrytime -= GetFrameTime();
			float cdist = goober.angrytime > 0 ? 6 : 4;
			if (Dist(state.bot_x, state.bot_y, goober.x, goober.y) < cdist) {
				if (goober.angrytime <= 0) {
					PlaySound(SND_DETECTION);
				}
				goober.angrytime = 3;
			}
			float pxd = state.bot_x - goober.x;
			float pyd = state.bot_y - goober.y;
			float pd = sqrtf(pxd * pxd + pyd * pyd);
			float hd = sqrtf(goober.x * goober.x + goober.y * goober.y);
			if (goober.angrytime < 0 || !state.bot_alive || PlayerHome(state) || pd > 7 || (hd * 2 < pd && hd > 3.f)) {
				goober.x -= (goober.x / hd) * GetFrameTime() * 0.3f;
				goober.y -= (goober.y / hd) * GetFrameTime() * 0.3f;
			}
			else if (goober.angrytime > 0) {
				pxd /= pd;
				pyd /= pd;
				goober.x += pxd * GetFrameTime() * 1.3f;
				goober.y += pyd * GetFrameTime() * 1.3f;
			}
			if (pd < 0.7f) {
				state.bot_alive = false;
				SpawnGoober(goober);
				PlaySound(SND_DIE);
			}
			if (hd < 6) {
				state.danger = true;
			}
			if (hd < 4) {
				state.flashing = true;
			}
			if (GooberHomeHome(goober)) {
				if (DisplayDeath(state)) {
					goto RESTART;
				}
				else {
					goto END;
				}
			}
		}
		if (!wasdanger && state.danger)
			PlaySound(SND_MENU);

		state.bat_err = false;
		if (state.bot_alive) {
			if (PlayerHome(state)) {
				state.bat_prog += GetFrameTime();
				state.life += 0.0008f;
				PlaySound(SND_PROGRESS);
			}
			else {
				state.bat_prog = 0;
				StopSound(SND_PROGRESS);
			}

			if (state.bat_prog >= BAT_DUR) {
				state.bat_prog = 0;
				state.bat++;
				StopSound(SND_PROGRESS);
				PlaySound(SND_REPAIR);
			}

			if (OnBrokeTile(state)) {
				if (state.bat > 0) {
					state.rep_prog += GetFrameTime();
					PlaySound(SND_PROGRESS1);
				}
				else {
					state.bat_err = true;
				}
			}
			else {
				state.rep_prog = 0;
				StopSound(SND_PROGRESS1);
			}

			if (state.rep_prog >= REP_DUR) {
				state.rep_prog = 0;
				state.bat--;
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
		} else {
			state.bot_respawn_timer += GetFrameTime();
			state.bat_prog = 0;

			if (state.bot_respawn_timer >= 3.f) {
				state.bot_x = 0;
				state.bot_y = 0;
				state.bot_alive = true;
				state.bot_respawn_timer = 0;
				state.bat = 0;
				state.life = 1;
			}
		}

		state.life = Clamp(state.life, 0, 1);

		{
			BeginTextureMode(map);

			ClearBackground(DARKBLUE);

			Camera3D cam{ 0 };
			cam.fovy = 10;
			cam.projection = CAMERA_ORTHOGRAPHIC;
			cam.up.y = 1;
			cam.position = { 5 + state.bot_x, 5, 5 + state.bot_y };
			cam.target = { state.bot_x, 0, state.bot_y };

			BeginMode3D(cam);

			DrawGrid(4 * GAME_SIZE, 0.5F);

			DrawPlane({ (float)state.btx, 0, (float)state.bty }, { 1, 1 }, state.bat > 0 ? YELLOW : BROWN);

			DrawCube({ 0, 0.5f, 0 }, 2, 1, 3, PlayerHome(state) ? GREEN : DARKGREEN);

			DrawLine3D({ state.bot_x, 0.5f, state.bot_y }, { state.bot_x + sinf(state.bot_rot), 0.5f, state.bot_y + cosf(state.bot_rot) }, RED);

			DrawSphere({ state.bot_x, 0.5f, state.bot_y }, 0.5f, state.bot_alive ? GREEN : GRAY);

			for (int i = 0; i < gooberc; i++) {
				Goober &goober = goobers[i];
				DrawSphere({ goober.x, 0.5f, goober.y }, 0.4f, goober.angrytime > 0 ? RED : DARKGRAY);
			}

			if (state.expr > 0) {
				DrawSphere({ state.expx, 0.5f, state.expy }, 0.6f, Fade(ORANGE, 0.3f * (state.expr / EXP_DUR)));
			}

			EndMode3D();

			if (state.danger && !(state.flashing && state.fstate)) {
				if (state.flashing)
					PlaySound(SND_MENU);
				Vector2 hPos = GetWorldToScreen({ 0, 0.5f, 0 }, cam);
				
				hPos.x = Clamp(hPos.x, 15, MAP_WIDTH - 15 - 15);
				hPos.y = Clamp(hPos.y, 15, MAP_HEIGHT - 15 - 15);

				DrawRectangle(hPos.x, hPos.y, 15, 15, RED);
			}
			if (state.bat > 0) {
				Vector2 btPos = GetWorldToScreen({ (float)state.btx, 0, (float)state.bty }, cam);

				btPos.x = Clamp(btPos.x, 15, MAP_WIDTH - 15 - 15);
				btPos.y = Clamp(btPos.y, 15, MAP_HEIGHT - 15 - 15);

				DrawRectangle(btPos.x, btPos.y, 15, 15, YELLOW);
			}

			DrawText(TextFormat("SCORE %d", state.rep_tiles), 15, 15, 20, WHITE);
			DrawText(TextFormat("PART %d", state.bat), 15, 40, 20, WHITE);
			DrawText(TextFormat("BAT %d", (int)ceil(state.life * 100)), 15, 65, 20, WHITE);

			if (state.bat_prog > 0) {
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200, 20, BLACK);
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200 * (state.bat_prog / BAT_DUR), 20, WHITE);
			}
			if (state.rep_prog > 0) {
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200, 20, BLACK);
				DrawRectangle((MAP_WIDTH - 200) / 2, MAP_HEIGHT / 2 - 50, 200 * (state.rep_prog / REP_DUR), 20, YELLOW);
			}
			if (state.bat_err) {
				const char *t = "No Substitute Part!";
				int w = MeasureText(t, 20);
				DrawText(t, (MAP_WIDTH - w) / 2, MAP_HEIGHT / 2 - 50, 20, RED);
			}

			if (!state.bot_alive) {
				DrawRectangle(0, 0, 800, 600, Fade(BLACK, 0.4f));
				const char *t = TextFormat("Respawning in %0.1f", 3.f - state.bot_respawn_timer);
				int w = MeasureText(t, 50);
				DrawText(t, (MAP_WIDTH - w) / 2, (MAP_HEIGHT - 50) / 2, 50, RED);
			}

			EndTextureMode();

		}

		BeginDrawing();
		
		ClearBackground(GRAY);

		DrawTextureRec(map.texture, { 0, 0, MAP_WIDTH, -MAP_HEIGHT }, { 25, 25 }, WHITE);

		DrawKeybindBar("[W] Forward [A]/[D] Rotate", "");

		DoFadeOutAnimation(fadein);

		EndDrawing();
	}

END:
	if (state.rep_tiles > globstate.hscore)
		globstate.hscore = state.rep_tiles;
	SaveGlobState();

	return false;
RESTART:
	if (state.rep_tiles > globstate.hscore)
		globstate.hscore = state.rep_tiles;
	SaveGlobState();

	UnloadRenderTexture(map);

	return true;
}

int main() {
	{
		R r = RRead("save.dat");
		if (r.file) {
			SER_REV(r);
			SERIALIZE(r, globstate);
		}
		RClose(r);
	}

	InitWindow(800, 600, "Moongram");
	InitAudioDevice();
	LoadSounds();
	SetExitKey(0);

	SetTargetFPS(30);

	if (!PickFlags())
		goto END;

	while (RunGame());

END:
	CloseWindow();
}