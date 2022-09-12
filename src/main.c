#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../deps/tomlc99/toml.h"

void error(const char* err, const char* info) {
	fprintf(stderr, "%s%s%s\n", err, info?"\n":"", info?info:"");
	exit(1);
}

typedef struct {
	char* name;
} build_config;

build_config* load_config() {
	FILE* fp;
	char errbuf[200];
	build_config* conf = malloc(sizeof(build_config));

	fp = fopen("freight.toml", "r");
	if (!fp) { error("No freight.toml found!", NULL); }

	toml_table_t* conf_raw = toml_parse_file(fp, errbuf, sizeof(errbuf));
	fclose(fp);
	if (!conf_raw) { error("Could not parse toml file!", errbuf); }

	toml_table_t* package = toml_table_in(conf_raw, "package");
	if (!package) { error("Could not find [package]!", NULL); }

	toml_datum_t package_name = toml_string_in(package, "name");
	if (!package_name.ok) { error("No package name specified!", NULL); }
	conf->name = malloc(strlen(package_name.u.s)+1);
	strcpy(conf->name, package_name.u.s);
	free(package_name.u.s);

	return conf;
}

void build() {
	build_config* config;
	config = load_config();
	printf("name: %s\n", config->name);
}

void run() {
	build();
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Freight help: not yet implemented!\n");
	} else {
		if (strcmp(argv[1], "build") == 0) {
			build();
		} else if (strcmp(argv[1], "run") == 0) {
			run();
		} else {
			printf("Unknown argument specified\n");
		}
	}
	return 0;
}
