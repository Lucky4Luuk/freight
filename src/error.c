#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void error(const char* err, const char* info) {
	fprintf(stderr, "%s%s%s\n", err, info?"\n":"", info?info:"");
	exit(1);
}
