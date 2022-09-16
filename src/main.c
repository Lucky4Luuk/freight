#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "error.h"
#include "util.h"
#include "config.h"
#include "../deps/dirent/src/dirent.h"
#include "../deps/tomlc99/src/toml.h"
#include "../deps/stb_ds/src/stb_ds.h"

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

void build_dependencies(build_config* config, char*** vector) {
	printf("Building %d dependencies...\n", config->dep_vec_len);
	cross_chdir("deps");
	for (int i = 0; i < config->dep_vec_len; i++) {
		printf("  Building %s v%s\n", config->dep_vec[i], config->dep_version_vec[i]);
		cross_chdir(config->dep_vec[i]);
		system("freight build");
		cross_chdir("..");
		int needed = format_length("deps/%s/%s.o", config->dep_vec[i], config->dep_vec[i]);
		char* s = malloc(sizeof(char) * needed);
		sprintf(s, "deps/%s/%s.o", config->dep_vec[i], config->dep_vec[i]);
		arrpush(*vector, s);
	}
	cross_chdir("..");
}

void build(build_config* config) {
	// Test if the right folders exist
	if (!dir_exists("deps")) { error("Project setup is wrong!", "`deps` folder is missing!"); }
	if (!dir_exists("src")) { error("Project setup is wrong!", "`src` folder is missing!"); }

	// Build self and dependencies
	char** vector = NULL;
	build_dependencies(config, &vector);
	printf("Building %s...\n", config->name);
	build_recursive(config, "src", &vector);
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

	// Construct and invoke build command
	if (config->lib == 1) {
		needed = format_length("%s %s %s -c -o %s.o", config->compiler, config->cflags, concat, config->name);
		char* cmd = malloc(sizeof(char) * needed);
		sprintf(cmd, "%s %s %s -c -o %s.o", config->compiler, config->cflags, concat, config->name);
		printf("Invoking `%s`\n", cmd);
		system(cmd);
	} else {
		#if defined(_WIN32)
			needed = format_length("%s %s %s -o %s.exe", config->compiler, config->cflags, concat, config->name);
			char* cmd = malloc(sizeof(char) * needed);
			sprintf(cmd, "%s %s %s -o %s.exe", config->compiler, config->cflags, concat, config->name);
		#else
			needed = format_length("%s %s %s -o %s", config->compiler, config->cflags, concat, config->name);
			char* cmd = malloc(sizeof(char) * needed);
			sprintf(cmd, "%s %s %s -o %s", config->compiler, config->cflags, concat, config->name);
		#endif
		printf("Invoking `%s`\n", cmd);
		system(cmd);
	}
}

void run(build_config* config) {
	build(config);
	printf("Running program...\n");
	#if defined(_WIN32)
		int needed = format_length("%s.exe", config->name);
		char* cmd = malloc(needed);
		sprintf(cmd, "%s.exe", config->name);
		system(cmd);
	#else
		int needed = format_length("./%s", config->name);
		char* cmd = malloc(needed);
		sprintf(cmd, "./%s", config->name);
		system(cmd);
	#endif
}

struct stat st = {0};

int main(int argc, char *argv[]) {
	const char* HELP_STRING = "Freight\n  help - shows this\n  new <name> - creates a new project\n  build - builds project in current directory\n  run - builds and runs project in current directory\n";
	if (argc < 2) {
		printf("%s", HELP_STRING);
	} else {
		if (strcmp(argv[1], "help") == 0) {
			printf("%s", HELP_STRING);
		} else if (strcmp(argv[1], "new") == 0) {
			if (argc < 3) {
				error("No name provided!", NULL);
			}
			printf("Creating project named %s...\n", argv[2]);
			if (stat(argv[2], &st) != -1) {
				error("Directory already exists!", NULL);
			}
			cross_mkdir(argv[2]);
			cross_chdir(argv[2]);
			FILE* f = fopen("freight.toml", "w");
			fprintf(f, "[package]\nname = \"%s\"\ncompiler = \"clang\"\ncflags = \"-std=c99\"", argv[2]);
			fclose(f);
			cross_mkdir("deps");
			cross_mkdir("src");
			cross_chdir("src");
			f = fopen("main.c", "w");
			fprintf(f, "#include <stdio.h>\n\nint main() {\n\tprintf(\"Hello, world!\\n\");\n\treturn 0;\n}");
			fclose(f);
			printf("Project created!\n");
		} else if (strcmp(argv[1], "build") == 0) {
			build_config* config;
			config = load_config();
			build(config);
			conf_free(config);
		} else if (strcmp(argv[1], "run") == 0) {
			build_config* config;
			config = load_config();
			run(config);
			conf_free(config);
		} else {
			printf("Unknown argument specified\n");
		}
	}
	return 0;
}
