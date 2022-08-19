// #define _STD_BUILD_VERBOSE
#include "module/build.h"

#include "stdbuild"

// #include "module2/build.h"

struct project : std::build::package {
	project() {
		name = "project";
		dir = "project/";
		flags = { "-std=c++20", "-DTEST_DEF" };
		sources = { "src/main.cpp", "src/test.cpp" };
		pkgs = { Module("module"), Module2("module2") };
	}
};

int main() {
	auto proj = project();
	std::build::create_executable(proj);
}