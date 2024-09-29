#include "global.h"

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

		const char *s = "Your home base has been infiltrated!";
		int m = MeasureText(s, 30);
		DrawText(s, (800 - m) / 2, (600 - 60) / 2, 30, WHITE);
		s = TextFormat("Before that, you repaired %d %s", state.rep_tiles, state.rep_tiles == 1 ? "tile" : "tiles");
		m = MeasureText(s, 30);
		DrawText(s, (800 - m) / 2, (600) / 2, 30, WHITE);
		if (state.rep_tiles > globstate.hscore_up) {
			s = "(That's a new high score!)";
			m = MeasureText(s, 30);
			DrawText(s, (800 - m) / 2, (600 + 60) / 2, 30, WHITE);
		}

		DrawKeybindBar("", "[Enter] Restart", false);

		EndDrawing();
	}

	return false;
}