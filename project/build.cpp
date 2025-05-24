// This is an example project used to testing and demonstration purposes.
// It is likely to change heavily over time.

#include <stdbuild.h>
#include "packages/MyLibrary/build.h"

using namespace stdbuild;

struct project : package {
	project() {
		name = "project";

		if(is_gcc() || is_clang()) {
			flags = { "-std=c++20", "-DTEST_DEF", "-Wall", "-Wextra", "-Wshadow", "-Wconversion", "-Wpedantic", "-Werror" };
		} else if(is_msvc()) {
			flags = { "/std:c++20", "/permissive", "/W4", "/w14640", "/EHsc" };
		}

		sources = { {"src/main.cpp"}, {"src/test.cpp"} };
		pkgs = { MyLibrary("packages/MyLibrary") };
	}
};

int main() {
	enable_verbose_output();
	// disable_caching();
	create<project>();
}