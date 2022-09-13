#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "error.h"
#include "util.h"
#include "config.h"
#include "../deps/tomlc99/toml.h"
#include "../deps/stb_ds/stb_ds.h"

void build_recursive(build_config* config, char* basepath, char*** vector) {
	char path[1000];

	struct dirent* dp;
	DIR* dir = opendir(basepath);
	if (!dir) { return; }

	while ((dp = readdir(dir)) != NULL) {
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
			if (ends_with(dp->d_name, ".c")) {
				int needed = format_length("%s/%s", basepath, dp->d_name);
				char* s = malloc(sizeof(char) * needed);
				sprintf(s, "%s/%s", basepath, dp->d_name);
				arrpush(*vector, s);
			}

			strcpy(path, basepath);
			strcat(path, "/");
			strcat(path, dp->d_name);

			build_recursive(config, path, vector);
		}
	}

	closedir(dir);
}

void build(build_config* config) {
	printf("Building %s...\n", config->name);
	char** vector = NULL;
	build_recursive(config, "src", &vector);
	build_recursive(config, "deps", &vector);
	size_t needed = 0;
	for (int i = 0; i < arrlen(vector); ++i)
		needed += strlen(vector[i]) + 1;
	char* concat = malloc(sizeof(char) * needed);
	if (!concat) { error("Failed to allocate string!", NULL); }
	strcpy(concat, vector[0]);
	strcat(concat, " ");
	for (int i = 1; i < arrlen(vector); ++i) {
		strcat(concat, vector[i]);
		strcat(concat, " ");
	}
	arrfree(vector);

	// Build command
	needed = format_length("%s %s %s -o %s", config->compiler, config->cflags, concat, config->name);
	char* cmd = malloc(sizeof(char) * needed);
	sprintf(cmd, "%s %s %s -o %s", config->compiler, config->cflags, concat, config->name);
	printf("Invoking `%s`\n", cmd);
	system(cmd);
}

void run(build_config* config) {
	build(config);
	int needed = format_length("./%s", config->name);
	char* cmd = malloc(needed);
	sprintf(cmd, "./%s", config->name);
	system(cmd);
}

int main(int argc, char *argv[]) {
	const char* HELP_STRING = "Freight\n\thelp - shows this\n\tbuild - builds project in current directory\n\trun - builds and runs project in current directory\n";
	if (argc < 2) {
		printf("%s", HELP_STRING);
	} else {
		if (strcmp(argv[1], "help") == 0) {
			printf("%s", HELP_STRING);
		} else if (strcmp(argv[1], "build") == 0) {
			build_config* config;
			config = load_config();
			build(config);
		} else if (strcmp(argv[1], "run") == 0) {
			build_config* config;
			config = load_config();
			run(config);
		} else {
			printf("Unknown argument specified\n");
		}
	}
	return 0;
}
