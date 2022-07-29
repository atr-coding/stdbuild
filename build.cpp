#include "stdbuild"

#include "module/build.h"

using namespace std::build;

int main() {
	enable_debugging();

	auto module = module::build();
	
	project proj("project", type::executable);
	set_directory(proj, "project");
	add_flags(proj, "-std=c++20");
	add_include_directories(proj, "include/");
	add_source_files(proj, "src/main.cpp");
	add_dependencies(proj, module);
	create_executable(proj);
}


// std::string expanded_str_vector(const string_vector& str_vec, char delim = 0) {
// 	std::string output;
// 	for (const auto& str : str_vec) { output += str + delim; }
// 	return std::move(output);
// }
// build.add_pre_command("cppcheck " + expanded_str_vector(build.source_files(), ' '));