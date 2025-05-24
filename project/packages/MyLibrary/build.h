#include <stdbuild.h>

struct MyLibrary : stdbuild::package {
	MyLibrary(stdbuild::path path) {
		dir = path;
		name = "MyLibrary";
		include_dirs = { "include/" };
		library_dirs = { "bin/"};
		type = stdbuild::static_library;
		pre_built = true;
	}
};

stdbuild::package MyLib(stdbuild::path path) {
	stdbuild::package lib;
	lib.dir = path;
	lib.name = "MyLibrary";
	lib.include_dirs = { "include/" };
	lib.library_dirs = { "bin/" };
	lib.type = stdbuild::static_library;
	lib.sources = { "src/add.cpp" };
	lib.pre_built = false;
	return lib;
}