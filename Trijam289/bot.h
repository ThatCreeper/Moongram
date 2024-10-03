#pragma once

void MoveBotAndDrainLife(BotState &bot);
void ClipBot(BotState &bot);
void UpdateTimers(GameState &state);
void KillBot(BotState &bot, DeathCause cause);