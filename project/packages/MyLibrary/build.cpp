#include <stdbuild.h>

struct MyLibrary: stdbuild::package {
	MyLibrary() {
		name = "MyLibrary";
		include_dirs = { "include/" };
		sources = { "src/add.cpp" };
		type = stdbuild::static_library;
	}
};

int main() {
	auto project = MyLibrary();
	stdbuild::create_library(project);
}