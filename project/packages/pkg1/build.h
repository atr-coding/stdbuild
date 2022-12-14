#include "../../../stdbuild"
#include <cassert>

struct PKG1 : std::build::package {
	PKG1(std::build::path _dir) {
		name = "pkg1";
		dir = _dir;
		ver = { 0, 0, 1 };
		include_dirs = { "include/" };
		sources = { "src/test.cpp" };
		flags = { "-std=c++20" };
	}
};