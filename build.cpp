#include "stdbuild"

using namespace std::build;

struct Module : ProjectBase {
	Module(build_path _dir) {
		name = "module";
		dir = _dir;
		version = { 0, 0, 1 };

		include_dirs = { "include/" };
		sources = { "src/test.cpp" };
		flags = { "-std=c++20" };
	}
};

struct Project : ProjectBase {
	Project() {
		name = "project";
		dir = "project/";
		flags = { "-std=c++20" };
		sources = { "src/main.cpp", "src/test.cpp" };
		packages = { Module("module") };
	}
};

int main() {
	enable_debugging();
	enable_verbose();

	auto proj = Project();
	create_executable(proj);
}