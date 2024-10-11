#include "global.h"

bool UpdatedRunGame() {
	int fadein = 0;
	bool restart = false;
	GameState state{};
	RenderTexture2D map = LoadRenderTexture(MAP_WIDTH, MAP_HEIGHT);

	for (Goober &goober : state.goobers) {
		SpawnGoober(goober);
	}
	ChooseBrokenTile(state);

	PlaySound(SND_START);

	DoFadeOutAnimation();

	while (!WindowShouldClose()) {
		UpdateBot(state);

		if (UpdateGoobers(state)) {
			restart = DisplayDeath(state);
			goto END;
		}

		state.explosion.rem -= GetFrameTime();

		RenderMap(state, map);

		BeginDrawing();

		RenderScreen(map, state.bot);
		DoFadeInAnimation(fadein);

		EndDrawing();
	}

END:
	if (state.rep_tiles > globstate.hscore_up)
		globstate.hscore_up = state.rep_tiles;
	SaveGlobState();

	UnloadRenderTexture(map);

	return restart;
}