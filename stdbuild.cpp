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

// TODO: Improve this
std::string clean_string(const std::string& str) {
	std::string temp = str;
	temp.erase(std::remove_if(temp.begin(), temp.end(), [](const char& c) { return !std::isalnum(c); }), temp.end());
	return temp;
}

void generate_project_template(const std::string& name) {
	// Create directory structure.
	fs::create_directory("bin");
	fs::create_directory("build");
	fs::create_directory("include");
	fs::create_directory("src");
	fs::create_directory("lib");

	if(!fs::exists("src/main.cpp")) {
		std::stringstream main_file;
		main_file << "#include <iostream>\n\n";
		main_file << "int main() {\n";
		main_file << "\tstd::cout << \"Hello World\\n\";\n";
		main_file << "}";

		std::ofstream maincpp("src/main.cpp");
		if(maincpp.is_open()) {
			maincpp << main_file.str();
			maincpp.close();
		}
	}

	if(!fs::exists("build.cpp")) {
		auto cleaned_name = clean_string(name);

		std::stringstream build_file;
		build_file << "#include <stdbuild.h>\n\n";
		build_file << "using namespace stdbuild;\n\n";
		build_file << "struct " << cleaned_name << " : package {\n";
		build_file << '\t' << cleaned_name << "() {\n";
		build_file << "\t\tname = \"" << cleaned_name << "\";\n";
		build_file << "\t\tsources = { \"src/main.cpp\" };\n";
		build_file << "\t}\n";
		build_file << "};\n\n";
		build_file << "int main() {\n";
		build_file << "\tcreate<" << cleaned_name << ">();\n";
		build_file << "}";

		std::ofstream buildcpp("build.cpp");
		if(buildcpp.is_open()) {
			buildcpp << build_file.str();
			buildcpp.close();
		}
	}
}

void print_help() {
	std::cout << "usage: stdbuild [*.cpp] [-r | --run] [-v | --verbose] [-h | --help] [--create <name>] [-i] [-j n]\n";
	std::cout << "[*.cpp]             The last cpp file specified in the parameter list will be used as the build script.\n";
	std::cout << "                    If a cpp file is not specified, build.cpp will be used as the default.\n";
	std::cout << "[-h | --help]       Displays this.\n";
	std::cout << "[--create <name>]   Creates a basic project directory structure and a build script in the current\n";
	std::cout << "                    directory with the given name.\n";
	std::cout << "[-i]                Includes the directory where the stdbuild application (and the stdbuild header) live.\n";
}

int main(int argc, char** argv) {
	const fs::path stdbuild_path = home_directory().parent_path() / "include";
	bool include_stdbuild{ false };
	std::string build_file_path;

	if(std::filesystem::exists("build.cpp")) {
		build_file_path = "build.cpp";
	}

	// Check for run & verbose commands
	for(auto i = 0; i < argc; ++i) {
		std::string arg{ argv[i] };
		if(arg.ends_with(".cpp")) {
			build_file_path = arg;
		} else if(arg == "--create") {
			std::cout << "Create project structure - ";
			if(i + 1 < argc) {
				std::string name{ argv[i + 1] }; // TODO: Clean this input.
				if(name.at(0) != '-' && !name.ends_with(".cpp")) {
					generate_project_template(name);
					std::cout << "complete.\n";
				} else {
					std::cout << "failed.\n";
				}
			}
			return 0;
		} else if(arg == "-i") {
			include_stdbuild = true;
		} else if(arg == "-h" || arg == "--help") {
			print_help();
			return 0;
		}
	}

	if(build_file_path.size() == 0) {
		std::cout << "No build file found.\n";
		return 1;
	}

	std::ostringstream ss;
	ss << "g++ ";

	if(include_stdbuild) {
		ss << "-I" << stdbuild_path << ' ';
	}

	ss << build_file_path;

	ss << " -o build.exe";

	int ret = std::system(ss.str().c_str());

	if(std::filesystem::exists("build.exe") && ret == 0) {
		int ret = std::system("build");
		if(ret) {
			return 1; // cause stdbuild to fail if the build fails
		}
	} else {
		std::cout << "Failed to generate build program.\n";
	}
}
