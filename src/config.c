#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "../deps/tomlc99/toml.h"
#include "../deps/dirent/dirent.h"
#include "../deps/stb_ds/stb_ds.h"
#include "util.h"

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

	toml_datum_t lib = toml_bool_in(package, "lib");
	if (!lib.ok) {
		conf->lib = 0;
	} else {
		conf->lib = lib.u.b;
	}

	toml_table_t* deps = toml_table_in(conf_raw, "dependencies");
	if (deps) {
		int arr_len = 0;
		for (int i = 0; ; i++) {
			const char* key = toml_key_in(deps, i);
			if (!key) break;
			toml_datum_t value = toml_string_in(deps, key);
			if (!value.ok) { error("Illegal value in toml!", NULL); }
			int needed = format_length("%s\0", key);
			char* key_fixed = malloc(sizeof(char) * needed);
			sprintf(key_fixed, "%s\0", key);
			arrpush(conf->dep_vec, key_fixed);
			arrpush(conf->dep_version_vec, value.u.s);
			arr_len++;
		}
		conf->dep_vec_len = arr_len;
	} else {
		conf->dep_vec = NULL;
		conf->dep_version_vec = NULL;
		conf->dep_vec_len = 0;
	}

    toml_free(conf_raw);

	return conf;
}

void conf_free(build_config* conf) {
	// I don't know if this is needed
	// for (int i = 0; i < conf->dep_vec_len; i++) {
	// 	free(conf->dep_vec[i]);
	// 	free(conf->dep_version_vec[i]);
	// }
	arrfree(conf->dep_vec);
	arrfree(conf->dep_version_vec);
}
