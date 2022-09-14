#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../deps/dirent/dirent.h"

#include "error.h"
#include "../deps/tomlc99/toml.h"

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

    toml_datum_t compiler = toml_string_in(package, "compiler");
	if (!compiler.ok) { error("No compiler specified!", NULL); }
	conf->compiler = malloc(strlen(compiler.u.s)+1);
	strcpy(conf->compiler, compiler.u.s);
	free(compiler.u.s);

    toml_datum_t cflags = toml_string_in(package, "cflags");
	if (!cflags.ok) { error("No cflags specified!", NULL); }
	conf->cflags = malloc(strlen(cflags.u.s)+1);
	strcpy(conf->cflags, cflags.u.s);
	free(cflags.u.s);

    toml_free(conf_raw);

	return conf;
}
