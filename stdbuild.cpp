// A program used to automatically run build.cpp

#include <filesystem>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char** argv) {
	bool run_after_build = false;
	bool verbose = false;
	std::string build_file_path;

	if (std::filesystem::exists("build.cpp")) {
		build_file_path = "build.cpp";
	}

	// Check for run & verbose commands
	for (auto i = 0; i < argc; ++i) {
		std::string arg{ argv[i] };
		if (arg == "-r" || arg == "-run") {
			run_after_build = true;
		} else if (arg == "-v" || arg == "-verbose") {
			verbose = true;
		} else if (arg.ends_with(".cpp")) {
			build_file_path = arg;
		}
	}

	if(build_file_path.size() == 0) {
		std::cout << "No build file found.\n";
		return 1;
	}

	std::ostringstream ss;
	ss << "g++ -std=c++20 " << build_file_path;
	if (run_after_build) {
		ss << " -D_STD_BUILD_RUN";
	}
	if (verbose) {
		ss << " -D_STD_BUILD_VERBOSE";
	}
	ss << " -o stdbuild-autogen";

	int ret = std::system(ss.str().c_str());

	if (std::filesystem::exists("stdbuild-autogen.exe") && ret == 0) {
		std::system("stdbuild-autogen");
		std::system("del stdbuild-autogen.exe");
	} else {
		std::cout << "Failed to generate stdbuild-autogen.\n";
	}
}