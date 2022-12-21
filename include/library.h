#pragma once

#include "base.h"
#include "options.h"

namespace _STD_BUILD {

	struct _Library_Output {
		fs::path name;
		library_type type{ static_library };
	};

	inline _Library_Output create_library(package& pkg) {
		_STD_BUILD_OUTPUT(pkg.name << " - " << (pkg.type == header_library ? "header\n" : (pkg.type == static_library ? "static" : "shared")));

		if(pkg.type != header_library && pkg.pre_built == false) {
			_STD_BUILD_VERBOSE_OUTPUT('\n');
		}

		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);

		auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto project_build_dir = build_dir / pkg.name;

		pkg.pre();

		// Transform libraries include directories by adding the project directory to the beginning.
		for(auto& id : pkg.include_dirs) {
			id = { (pkg.dir / id.value).string(), id.access_level };
		}

		// Transform libraries library directories by adding the project directory to the beginning.
		for(auto& ld : pkg.library_dirs) {
			ld = { (pkg.dir / ld.value).string(), ld.access_level };
		}

		if(pkg.pre_built) {
			_STD_BUILD_OUTPUT(" - prebuilt\n");
			return { pkg.name, pkg.type };
		}

		if(pkg.type != header_library) {
			if(pkg.sources.size() > 0) {
				// Compile all the libraries source files.
				// If an error occurs with one of the compilations, then an exception will propagate up and terminate the build.
				compile(project_build_dir, pkg);

				// Convert all source file names into corresponding object file names.
				std::stringstream obj_files;
				for(const auto& source : pkg.sources) {
					obj_files << (project_build_dir / (source.value.stem().replace_extension(".o"))).string() << ' ';
				}

				_STD_BUILD_VERBOSE_OUTPUT("Linking...");

				auto file = fs::path("lib" + pkg.name + (pkg.type == static_library ? ".a" : ".so"));
				std::stringstream output;

				if(pkg.type == static_library) {
					output << "ar rcs ";
				} else if(pkg.type == shared_library) {
					output << _STD_BUILD_COMPILER << " -shared -o ";
				}
				output << bin_dir / file << ' ' << obj_files.str();

				_STD_BUILD_VERBOSE_OUTPUT('\n');
				if(command(output.str())) {
					throw library_exception("There was an error in creating the library: " + pkg.name, true);
				}
				_STD_BUILD_VERBOSE_OUTPUT("output");
				_STD_BUILD_OUTPUT(" - " << (bin_dir / file).string() << '\n');

				pkg.post();

				return { pkg.name, pkg.type };
			} else {
				_STD_BUILD_VERBOSE_OUTPUT("No sources given, assuming header only.\n");
			}
		}

		pkg.post();

		return { pkg.name, header_library };
	}
} // namespace _STD_BUILD