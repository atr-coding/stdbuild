// A program used to automatically run build.cpp

#include <filesystem>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <fstream>
#include <algorithm>
#include <Windows.h>

namespace fs = std::filesystem;

fs::path home_directory() {
	char* buffer = new char[2048];
	std::size_t buffer_size = 2048;
	std::size_t path_length = GetModuleFileNameA(NULL, buffer, buffer_size);
	std::string str(buffer, path_length);
	delete buffer;
	return fs::path(str);
}

std::string clean_string(const std::string& str) {
	std::string temp = str;
	temp.erase(std::remove_if(temp.begin(), temp.end(), [](const char& c) { return !std::isalnum(c); }), temp.end());
	return temp;
}

bool generate_project_template(const fs::path& name) {
	if (!fs::exists(name)) {
		// Create directory structure.
		fs::create_directory(name);
		fs::create_directory(name / "bin");
		fs::create_directory(name / "build");
		fs::create_directory(name / "include");
		fs::create_directory(name / "src");
		fs::create_directory(name / "lib");

		std::stringstream main_file;
		main_file << "#include <iostream>\n\n";
		main_file << "int main() {\n";
		main_file << "\tstd::cout << \"Hello World\\n\";\n";
		main_file << "}";

		auto cleaned_name = clean_string(name.string());

		std::stringstream build_file;
		build_file << "#include <stdbuild>\n\n";
		build_file << "struct " << cleaned_name << " : std::build::package {\n";
		build_file << '\t' << cleaned_name << "() {\n";
		build_file << "\t\tname = \"" << cleaned_name << "\";\n";
		build_file << "\t\tsources = { \"src/main.cpp\" };\n";
		build_file << "\t}\n";
		build_file << "};\n\n";
		build_file << "int main() {\n";
		build_file << "\tauto project = " << cleaned_name << "();\n";
		build_file << "\tstd::build::create_executable(project);\n";
		build_file << "}";

		std::ofstream maincpp(name / "src/main.cpp");
		if (maincpp.is_open()) {
			maincpp << main_file.str();
			maincpp.close();
		}

		std::ofstream buildcpp(name / "build.cpp");
		if (buildcpp.is_open()) {
			buildcpp << build_file.str();
			buildcpp.close();
		}

		return fs::exists(name) && fs::exists(name / "bin") && fs::exists(name / "build") &&
		       fs::exists(name / "include") && fs::exists(name / "src") && fs::exists(name / "lib") &&
		       fs::exists(name / "src/main.cpp") && fs::exists(name / "build.cpp");
	}
	return false;
}

int main(int argc, char** argv) {
	bool run_after_build = false;
	bool verbose = false;
	const fs::path stdbuild_path = home_directory().parent_path();
	bool include_stdbuild{ false };
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
		} else if (arg == "-new") {
			std::cout << "Generating project structure - ";
			if (i + 1 < argc) {
				std::string name{ argv[i + 1] }; // TODO: Clean this input.
				if (name.at(0) != '-') {
					std::cout << (generate_project_template(fs::path(name)) ? "done." : "failed.") << '\n';
				}
			}
			return 0;
		} else if (arg == "-i") {
			include_stdbuild = true;
		}
	}

	if (build_file_path.size() == 0) {
		std::cout << "No build file found.\n";
		return 1;
	}

	std::ostringstream ss;
	ss << "g++ -std=c++20 ";
	if (include_stdbuild) {
		ss << "-I" << stdbuild_path << ' ';
	}
	ss << build_file_path;
	if (run_after_build) {
		ss << " -D_STD_BUILD_RUN";
	}
	if (verbose) {
		ss << " -D_STD_BUILD_VERBOSE";
	}
	ss << " -o stdbuild-autogen";

	int ret = std::system(ss.str().c_str());

	if (std::filesystem::exists("stdbuild-autogen.exe") && ret == 0) {
		int ret = std::system("stdbuild-autogen");
		if (ret) {
			return 1; // cause stdbuild to fail if the build fails
		}
		std::system("del stdbuild-autogen.exe");
	} else {
		std::cout << "Failed to generate stdbuild-autogen.\n";
	}
}