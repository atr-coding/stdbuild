#include "../../../stdbuild"
#include <cassert>

struct PKG1 : stdbuild::package {
	PKG1(stdbuild::path _dir) {
		name = "pkg1";
		dir = _dir;
		ver = { 0, 0, 1 };
		include_dirs = { "include/" };
		sources = { "src/test.cpp" };
		flags = { "-std=c++20" };
	}
};