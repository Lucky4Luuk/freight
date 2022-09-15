typedef struct {
	char* name;
	char* compiler;
	char* cflags;
	int lib;
	int dep_vec_len;
	char** dep_vec;
	char** dep_version_vec;
} build_config;

build_config* load_config();
void conf_free(build_config*);
