// This is an example project used to testing and demonstration purposes.
// It is likely to change heavily over time.

#include "packages/pkg1/build.h"
#include "packages/pkg2/build.h"
#include "../stdbuild"

struct project : std::build::package {
	project() {
		name = "project";
		dir = "project";
		flags = { "-std=c++20", "-DTEST_DEF" };
		sources = { "src/main.cpp", "src/test.cpp" };
		pkgs = { PKG1("project/packages/pkg1"), PKG2("project/packages/pkg2") };
	}
};

int main() {
	auto proj = project();
	std::build::create_executable(proj);
}