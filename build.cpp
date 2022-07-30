#include <iostream>
#include <vector>
#include <concepts>
#include "stdbuild"

#include "module/build.h"

using namespace std::build;

int main(int argc, char** argv) {

	List sources = {
		"src/main.cpp",
		"src/test.cpp"
	};

	// enable_debugging();

	// auto module = module::build();

	// std::vector<std::string> files = { "src/main.cpp", "src/test.cpp" };

	// project proj("project", type::executable);
	// set_directory(proj, "project");
	// add_flags(proj, "-std=c++20", "-DTEST_DEF");
	// add_include_directories(proj, "include/");
	// add_source_files(proj, files);
	// add_dependencies(proj, module);
	// set_version(proj, { 0, 0, 1 });
	// create_executable(proj);


	/*

	IncludeDirectoryList incs = {
		{"include/", true}
	};

	proj.add(IncludeDirectory("include/", true), Flags("-std=c++20", "-DTEST_DEF"), Files(files), Package(module), Library("OpenGL32.lib", true));
	proj.set_version({0, 0, 1});

	*/
}


// std::string expanded_str_vector(const string_vector& str_vec, char delim = 0) {
// 	std::string output;
// 	for (const auto& str : str_vec) { output += str + delim; }
// 	return std::move(output);
// }
// build.add_pre_command("cppcheck " + expanded_str_vector(build.source_files(), ' '));