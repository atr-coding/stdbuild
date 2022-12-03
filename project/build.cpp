// This is an example project used to testing and demonstration purposes.
// It is likely to change heavily over time.

// #include "packages/pkg1/build.h"
// #include "packages/pkg2/build.h"
#include "../stdbuild"
#include "packages/packages.h"

const std::build::string_list warning_flags = {
	"-Wall", "-Wextra", "-Wshadow", "-Wconversion", "-Wpedantic", "-Werror"
};

struct project : std::build::package {
	project() {
		name = "project";
		flags = { "-std=c++20", "-DTEST_DEF", "-fanalyzer" };
		flags += warning_flags;
		sources = { "src/main.cpp", "src/test.cpp" };
		pkgs = { packages };
	}
};

int main() {
	auto proj = project();
	std::build::create_executable(proj);
}