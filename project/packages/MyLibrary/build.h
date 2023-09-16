#include <stdbuild.h>

struct MyLibrary : stdbuild::package {
	MyLibrary(stdbuild::path path) {
		dir = path;
		name = "MyLibrary";
		include_dirs = { { "include/", stdbuild::access::all } };
		library_dirs = { { "bin/", stdbuild::access::all } };
		type = stdbuild::static_library;
		pre_built = true;
	}
};

stdbuild::package MyLib(stdbuild::path path) {
	stdbuild::package lib;
	lib.dir = path;
	lib.name = "MyLibrary";
	lib.include_dirs = { { "include/", stdbuild::access::all } };
	lib.library_dirs = { { "bin/", stdbuild::access::all } };
	lib.type = stdbuild::static_library;
	lib.sources = { "src/add.cpp" };
	lib.pre_built = false;
	return lib;
}