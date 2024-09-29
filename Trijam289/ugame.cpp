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
		MoveBotAndDrainLife(state.bot);

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