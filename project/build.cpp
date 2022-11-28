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
		dir = "project";
		flags = { "-std=c++20", "-DTEST_DEF", "-fanalyze" };
		flags += warning_flags;
		sources = { "src/main.cpp", "src/test.cpp" };
		// pkgs = { PKG1("project/packages/pkg1"), PKG2("project/packages/pkg2") };
		pkgs = { packages };
	}
};

std::string sources_to_string(const std::build::package& pkg) {
	std::string output;
	for (const auto& i : pkg.sources) {
		output.append((pkg.dir / std::filesystem::path(i.value())).string() + " ");
	}
	return output;
}

int main() {
	auto proj = project();
	std::build::command("cppcheck " + sources_to_string(proj));
	std::build::create_executable(proj);
}