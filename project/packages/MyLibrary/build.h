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