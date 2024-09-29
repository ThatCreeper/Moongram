#pragma once

#define MAP_WIDTH (800 - 50)
#define MAP_HEIGHT (600 - 50 - 30)
#define GAME_SIZE 20

#define GOOBER_COUNT 8
#define GOOBER_INIT_COUNT 3

#define BAT_DUR 2
#define REP_DUR 3
#define EXP_DUR 1.5f

#include <raylib.h>
#include <cmath>
#include "version.h"
#include "sound.h"
#include "gfx.h"
#include "helpers.h"
#include "globstate.h"
#include "state.h"
#include "death.h"