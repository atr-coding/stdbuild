#include "stdbuild"

#include "module/build.h"

using namespace std::build;

int main() {
	enable_debugging();
	auto module = module::build();

	List sources = {
		"project/src/main.cpp",
		"project/src/test.cpp"
	};

	List flags = {
		"-std=c++20",
		"-DTEST_DEF"
	};

	List inc_dirs = {
		"project/include/"
	};

	// PackageList packages = {
	// 	module::build()
	// };
	
	project proj("project", type::executable);
	add_flags(proj, flags);
	add_include_directories(proj, inc_dirs);
	add_source_files(proj, sources);
	add_dependencies(proj, module);
	set_version(proj, { 0, 0, 1 });
	create_executable(proj);
}