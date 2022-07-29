#include "stdbuild"

#include "module/build.h"

using namespace std::build;

BuildInfo project() {
	BuildInfo build("project", Type::executable);
	build.dir = "project";
	build.flags.add("-std=c++20");
	build.include_dirs.add("include");
	build.files.add("src/main.cpp");
	return std::move(build);
}

int main() {
	set_bin_directory("bin/");
	set_build_directory("build/");
	enable_debugging();

	auto module = module::build();
	
	auto proj = project();
	proj.dependencies.add(module);
	
	create_executable(proj);
}


// std::string expanded_str_vector(const string_vector& str_vec, char delim = 0) {
// 	std::string output;
// 	for (const auto& str : str_vec) { output += str + delim; }
// 	return std::move(output);
// }
// build.add_pre_command("cppcheck " + expanded_str_vector(build.source_files(), ' '));