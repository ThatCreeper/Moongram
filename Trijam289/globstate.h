#pragma once

struct GlobState {
	int hscore = 0;
};
extern GlobState globstate;

DECL_SERIALIZER(GlobState)

void SaveGlobState();