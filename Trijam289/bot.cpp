#include "global.h"

void MoveBotAndDrainLife(BotState &bot) {
	if (!bot.alive)
		return;

	float dec = 0.0005f;
	bool forward = IsKeyDown(KEY_W);
	bool backward = gameflags.modified_movement ? IsKeyDown(KEY_S) : false;
	bool moving = forward ^ backward;
	bool can_rotate = gameflags.modified_movement || moving || IsPlayerHome(bot);
	float mult = backward ? -0.4f : 1;

	if (can_rotate && IsKeyDown(KEY_A)) {
		bot.rot += 2.f * GetFrameTime(); 
		dec = 0.0007f * mult;
	}
	if (can_rotate && IsKeyDown(KEY_D)) {
		bot.rot -= 2.f * GetFrameTime();
		dec = 0.0007f * mult;
	}

	if (moving) {
		if (bot.rep_prog > 0) {
			bot.rep_prog = 0;
			StopSound(SND_PROGRESS1);
		}
		bot.x += sinf(bot.rot) * GetFrameTime() * 5.f * mult;
		bot.y += cosf(bot.rot) * GetFrameTime() * 5.f * mult;
		bot.life -= dec;
	}

	bot.life -= 0.0003f;
}

void ClipBot(BotState &bot) {
	if (bot.x < -GAME_SIZE)
		bot.x = -GAME_SIZE;
	if (bot.y < -GAME_SIZE)
		bot.y = -GAME_SIZE;
	if (bot.x > GAME_SIZE)
		bot.x = GAME_SIZE;
	if (bot.y > GAME_SIZE)
		bot.y = GAME_SIZE;
}

static void UpdateHomeTimers(BotState &bot) {
	if (IsPlayerHome(bot)) {
		bot.bat_prog += GetFrameTime();
		bot.life += 0.0008f;
		PlaySound(SND_PROGRESS);
	}
	else {
		bot.bat_prog = 0;
		StopSound(SND_PROGRESS);
	}

	if (bot.bat_prog >= BAT_DUR) {
		bot.bat_prog = 0;
		bot.batteries++;
		StopSound(SND_PROGRESS);
		PlaySound(SND_REPAIR);
	}
}

static void UpdateBrokenTileTimers(GameState &state) {
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

static void UpdateRespawnTimers(GameState &state) {
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

void UpdateTimers(GameState &state) {
	state.popup.bat_err = false;
	if (state.bot.alive) {
		UpdateHomeTimers(state.bot);
		UpdateBrokenTileTimers(state);
	}
	else {
		UpdateRespawnTimers(state);
	}

	state.explosion.rem -= GetFrameTime();
}
