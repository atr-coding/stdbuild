#pragma once

#include "base.h"

namespace _STD_BUILD {

	struct _Library_Output {
		fs::path location;
		fs::path name;
		library_type type{ static_library };
		_Library_Output(fs::path _location, fs::path _name, library_type _type) : location(_location), name(_name), type(_type) {}
	};

	inline std::optional<_Library_Output> create_library(package& pkg) {
		_STD_BUILD_OUTPUT(pkg.name << " - " << (pkg.type == header_library ? "header\n" : (pkg.type == static_library ? "static" : "shared")));

		if(pkg.type != header_library) {
			_STD_BUILD_VERBOSE_OUTPUT('\n');
		}

		_verify_bin_and_build_directories(pkg.name);

		auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto project_build_dir = build_dir / pkg.name;

		pkg.pre();

		// Transform library include directories by adding the project directory to the beginning.
		for(auto& id : pkg.include_dirs) {
			id = { (pkg.dir / id.value).string(), id.access_level };
		}

		if(pkg.type != header_library) {
			if(pkg.sources.size() > 0) {
				auto comp = compile(project_build_dir, pkg.dir, pkg.flags, pkg.include_dirs, pkg.sources);
				if(!comp) {
					return std::nullopt;
				}

				// Convert all source file names into corresponding object file names.
				std::stringstream obj_files;
				for(const auto& source : pkg.sources) {
					obj_files << (project_build_dir / (source.value.stem().replace_extension(".o"))).string() << ' ';
				}

				auto file = fs::path("lib" + pkg.name);
				_STD_BUILD_VERBOSE_OUTPUT("Linking...");
				std::stringstream output;

				if(pkg.type == static_library) {
					file.replace_extension(".a");
					output << "ar rcs " << bin_dir / file << ' ' << obj_files.str();
				} else if(pkg.type == shared_library) {
					file.replace_extension(".so");
					output << _STD_BUILD_COMPILER << " -shared -o " << bin_dir / file << ' ' << obj_files.str();
				}

				_STD_BUILD_VERBOSE_OUTPUT('\n');
				if(command(output.str())) {
					std::cout << "\nThere was an error in creating the library:\n";
					_print_error_log();
					return std::nullopt;
				}
				_STD_BUILD_VERBOSE_OUTPUT("output");
				_STD_BUILD_OUTPUT(" - " << (bin_dir / file).string() << '\n');

				pkg.post();

				return _Library_Output(bin_dir, pkg.name, pkg.type);
			} else {
				_STD_BUILD_VERBOSE_OUTPUT("No sources given, assuming header only.\n");
			}
		}

		pkg.post();

		return _Library_Output(bin_dir, pkg.name, header_library);
	}
} // namespace _STD_BUILD