#include "global.h"

void KillBot(BotState &bot, DeathCause cause) {
	bot.alive = false;
	bot.death_cause = cause;
}

void UpdateBot(GameState &state)
{
	float dec = 0.0005f;
	bool forward = IsKeyDown(KEY_W);
	bool backward = gameflags.modified_movement ? IsKeyDown(KEY_S) : false;
	bool moving = forward ^ backward;
	bool can_rotate = gameflags.modified_movement || moving || IsPlayerHome(state.bot);
	float mult = backward ? -0.4f : 1;

	if (can_rotate && IsKeyDown(KEY_A)) {
		state.bot.rot += 2.f * GetFrameTime();
		dec = 0.0007f * mult;
	}
	if (can_rotate && IsKeyDown(KEY_D)) {
		state.bot.rot -= 2.f * GetFrameTime();
		dec = 0.0007f * mult;
	}

	if (moving) {
		if (state.bot.rep_prog > 0) {
			state.bot.rep_prog = 0;
			StopSound(SND_PROGRESS1);
		}
		state.bot.x += sinf(state.bot.rot) * GetFrameTime() * 5.f * mult;
		state.bot.y += cosf(state.bot.rot) * GetFrameTime() * 5.f * mult;
		state.bot.life -= dec;
	}

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
		KillBot(state.bot, DC_BATTERY);
		PlaySound(SND_DIE);
	}

	state.popup.bat_err = false;
	if (state.bot.alive) {
		// +PART timer
		if (IsPlayerHome(state.bot)) {
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
		// +SCORE timer
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
			state.bot.death_cause = DC_BUG;
		}
	}
}
