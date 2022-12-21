#pragma once

#include "base.h"
#include "command.h"
#include "package.h"

namespace _STD_BUILD {

	////////////// Compilation //////////////

	using _Dependency_Vec = std::vector<package*>;

	inline void _add_packages(_Dependency_Vec& dep, package& project) {
		for(auto& pkg : project.pkgs) {
			dep.push_back(&pkg);
			_add_packages(dep, pkg);
		}
	};

	inline _Dependency_Vec _build_dependency_vector(package& main_project) {
		_Dependency_Vec dep;
		_add_packages(dep, main_project);

		// Remove duplicates.
		std::sort(dep.begin(), dep.end(), [](const auto* a, const auto* b) { return (*a < *b); });
		dep.erase(std::unique(dep.begin(), dep.end(), [](const auto* a, const auto* b) { return (*a == *b); }), dep.end());

		return dep;
	}

	bool _verify_bin_and_build_directories(const std::string& project_name) {
		const auto& bin_dir = options().bin_dir;
		const auto& build_dir = options().build_dir / project_name;
		// Create the desired bin directory if it doesn't exist already.
		if(!fs::exists(bin_dir)) {
			try {
				fs::create_directories(bin_dir);
			} catch(std::exception& e) {
				_STD_BUILD_OUTPUT("Failed to create directory " << bin_dir.string() << '\n');
				_STD_BUILD_FAILURE_RETURN();
			}
		}

		// Create the desired build directory if it doesn't exist already.
		if(!fs::exists(build_dir)) {
			try {
				fs::create_directories(build_dir);
			} catch(std::exception& e) {
				_STD_BUILD_OUTPUT("Failed to create directory " << build_dir.string() << '\n');
				_STD_BUILD_FAILURE_RETURN();
			}
		}

		return true;
	}

	bool compile(const fs::path& build_dir,
	             const fs::path& directory,
	             const string_list& flags,
	             const path_list& include_dirs,
	             const fs::path& cpp_file) {
		std::stringstream output;

		output << _STD_BUILD_COMPILER << " -c " << flags;

		for(const auto& i : include_dirs) {
			output << "-I" << i << ' ';
		}

		output << directory / cpp_file << " -o " << (build_dir / cpp_file.stem()).replace_extension(".o");
		if(command(output.str())) {
			_STD_BUILD_OUTPUT("\nThere was an error during compilation:\n");
			_print_error_log();
			return false;
		}
		return true;
	}

	bool compile(const fs::path& build_dir,
	             const fs::path& directory,
	             const string_list& flags,
	             const path_list& include_dirs,
	             const path_list& cpp_files) {
		if(cpp_files.size() > 0) {
			_STD_BUILD_VERBOSE_OUTPUT("Compiling...\n");

			for(const auto& file : cpp_files) {
				if(compile(build_dir, directory, flags, include_dirs, file.value) == false) {
					return false;
				}
			}
		}
		return true;
	}

} // namespace stdbuild