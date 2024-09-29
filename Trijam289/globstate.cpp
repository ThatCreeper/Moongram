#include "global.h"

GlobState globstate;

SERIALIZER(GlobState) {
	ADD(SR_INIT, hscore);
	SER_CHECK;
	ADD(SR_ADD_UPDATED, hscore_up);
	SER_CHECK;
} SERIALIZER_END

void SaveGlobState() {
	R r = RWrite("save.dat");
	if (r.file) {
		SER_REV(r);
		SERIALIZE(r, globstate);
	}
	RClose(r);
}