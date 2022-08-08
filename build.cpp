#define _STD_BUILD_VERBOSE
#include "stdbuild"
#include "module/build.h"
#include "module2/build.h"

struct Project : std::build::Package {
	Project() {
		name = "project";
		dir = "project/";
		flags = { "-std=c++20", "-DTEST_DEF" };
		sources = { "src/main.cpp", "src/test.cpp" };
		packages = {
			Module("module"),
			Module2("module2")
		};
	}
};

int main() {
	auto proj = Project();
	std::build::create_executable(proj);
}