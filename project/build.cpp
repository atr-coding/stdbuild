// This is an example project used to testing and demonstration purposes.
// It is likely to change heavily over time.

#include "../stdbuild"
#include "packages/packages.h"

const stdbuild::string_list warning_flags = {
	"-Wall", "-Wextra", "-Wshadow", "-Wconversion", "-Wpedantic", "-Werror"
};

struct project : stdbuild::package {
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
	stdbuild::create_executable(proj);
}