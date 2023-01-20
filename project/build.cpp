// This is an example project used to testing and demonstration purposes.
// It is likely to change heavily over time.

#include "../include/stdbuild.h"
#include "packages/packages.h"

using namespace stdbuild;

struct project : package {
	project() {
		name = "project";
		flags = { "-std=c++20", "-DTEST_DEF" };

		if(is_gcc() || is_clang()) {
			flags += { "-Wall", "-Wextra", "-Wshadow", "-Wconversion", "-Wpedantic", "-Werror" };
		} else if(stdbuild::is_msvc()) {
			flags += { "/permissive", "/W4", "/w14640" };
		}

		sources = { "src/main.cpp", "src/test.cpp" };
		pkgs = { packages };
	}
};

int main() {
	enable_verbose_output();
	run_after_build(true);
	create<project>();
}