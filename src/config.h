typedef struct {
	char* name;
	char* compiler;
	char* cflags;
} build_config;

build_config* load_config();
