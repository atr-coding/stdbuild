#pragma once

#include "base.h"
#include "options.h"
#include "threadpool.h"

namespace _STD_BUILD {

	struct _Library_Output {
		fs::path name;
		library_type type{ static_library };
	};

	inline _Library_Output create_library(package& pkg) {

		// Check if our bin/build directories exist, and if not, create them.
		// Throws if the directories could not be created.
		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);

		auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto project_build_dir = build_dir / pkg.name;

		_STD_BUILD_OUTPUT("Building | Type: "
		                  << (pkg.type == header_library ?
		                          "Header Library" :
		                          (pkg.type == static_library ? "Static Library" : "Shared Library"))
		                  << " | Name: " << pkg.name << '\n');

		pkg.pre();

		// Transform libraries sources by adding the project directory to the
		// beginning.
		/*for(auto& source : pkg.sources) {
		    source.value = pkg.dir / source.value;
		}*/

		// Transform libraries include directories by adding the project
		// directory to the beginning.
		for(auto& id : pkg.include_dirs) { id = { (pkg.dir / id.value).string(), id.access_level }; }

		// Transform libraries library directories by adding the project
		// directory to the beginning.
		for(auto& ld : pkg.library_dirs) { ld = { (pkg.dir / ld.value).string(), ld.access_level }; }

		if(pkg.pre_built) {
			return { pkg.name, pkg.type };
		}

		if(pkg.type != header_library) {
			if(pkg.sources.size() > 0) {
				// Load Cache
				__cache::cache_storage cache;
				const auto cache_path = project_build_dir / "cache";
				path_list files_to_compile = __cache::load_cache(cache_path, cache, pkg);

				// Compile all the libraries source files.
				// If an error occurs with one of the compilations, then an
				// exception will propagate up and terminate the build.
				compile(project_build_dir, pkg, files_to_compile);

				// Convert all source file names into corresponding object file
				// names.
				std::stringstream obj_files;
				for(const auto& source : pkg.sources) {
					obj_files << (project_build_dir / (source.value.stem().replace_extension(".o"))) << ' ';
				}

				_STD_BUILD_VERBOSE_OUTPUT("Linking...\n");

				auto file = fs::path("lib" + pkg.name + (pkg.type == static_library ? ".a" : ".so"));
				std::stringstream output;

				if(pkg.type == static_library) {
					output << "ar rcs ";
				} else if(pkg.type == shared_library) {
					output << _STD_BUILD_COMPILER << " -shared -o ";
				}
				output << bin_dir / file << ' ' << obj_files.str();

				if(command(output.str()) != 0) {
					throw library_exception("There was an error in creating the library: " + pkg.name, true);
				}

				// Update cache
				if(options().use_caching) {
					cache.update(pkg.include_dirs);
					cache.write_to_file(cache_path);
				}

				pkg.post();

				return { pkg.name, pkg.type };
			} else {
				_STD_BUILD_VERBOSE_OUTPUT("No sources given, assuming header only.\n");
			}
		}

		pkg.post();

		return { pkg.name, header_library };
	}

	inline void create_executable(package& pkg) {
		// Check if our bin/build directories exist, and if not, create them.
		// Throws if the directories could not be created.
		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);

		const auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto pkg_build_dir = build_dir / pkg.name;

		// Verify that we have source files to work with.
		if(pkg.sources.size() == 0) {
			// _STD_BUILD_LOG_AND_FAIL("No source files were given to compile for package: \"" << pkg.name << "\"\n");
			throw executable_exception("No source files were given to compile for package: \"" + pkg.name + "\"");
		}

		pkg.pre();

		// Transform include & library directories by prepending the pkg's
		// working directory
		for(auto& id : pkg.include_dirs) { id = (pkg.dir / id.value); }
		for(auto& ld : pkg.library_dirs) { ld = (pkg.dir / ld.value); }

		// Add all dependency pointers to a vector and remove duplicates with
		// the same name.
		auto dep = _build_dependency_vector(pkg);

		// Create the libraries and add them to a vector for future iteration
		std::vector<_Library_Output> libraries;
		if(!dep.empty()) {
			// _STD_BUILD_VERBOSE_OUTPUT("Building Dependencies: ");

			// Just used to display the names of the dependencies
			// for([[maybe_unused]] const auto* p : dep) { _STD_BUILD_VERBOSE_OUTPUT(p->name << ' '); }

			// _STD_BUILD_VERBOSE_OUTPUT('\n');
			for(package* d : dep) {
				libraries.push_back(create_library(*d));

				// Add this dependencies include/lib directories/libraries to
				// our main pkg's include/lib directories
				for(const auto& inc_dir : d->include_dirs) {
					if(inc_dir.access_level == access::all) {
						pkg.include_dirs.add(inc_dir);
					}
				}

				for(const auto& lib_dir : d->library_dirs) {
					if(lib_dir.access_level == access::all) {
						pkg.library_dirs.add(lib_dir);
					}
				}

				for(const auto& lib : d->libraries) {
					if(lib.access_level == access::all) {
						pkg.libraries.add(lib);
					}
				}
			}
		}

		// Load Cache
		__cache::cache_storage cache;
		const auto cache_path = build_dir / pkg.name / "cache";
		path_list files_to_compile = __cache::load_cache(cache_path, cache, pkg);

		// if(options().use_caching) {
		//	_STD_BUILD_VERBOSE_OUTPUT("Loading cache: ");

		//	const auto [initialized, new_cache] =
		//__cache::initialize_cache(cache_path, pkg.include_dirs, pkg.sources);
		//	if(initialized) {
		//		files_to_compile = std::move(pkg.sources);
		//		cache = new_cache;
		//	} else {
		//		if(!cache.load_from_file(cache_path)) {
		//			files_to_compile = std::move(pkg.sources);
		//			_STD_BUILD_VERBOSE_OUTPUT("failed to load file.\n");
		//		} else {
		//			cache.test(pkg.sources);
		//			files_to_compile += cache.changes.added;
		//			files_to_compile += cache.changes.modified;
		//			_STD_BUILD_VERBOSE_OUTPUT("complete [" <<
		// files_to_compile.size() + cache.changes.removed.size() << "
		// changes]\n");
		//		}
		//	}
		//} else {
		//	files_to_compile = std::move(pkg.sources);
		//}

		_STD_BUILD_OUTPUT("Building | Type: Executable     | Name: " << pkg.name << '\n');

		// Add debug flag
		if(debugging_enabled()) {
			pkg.flags += "-g";
		}

		compile(pkg_build_dir, pkg, files_to_compile);

		std::stringstream lds; // Library Directory String
		std::stringstream ls;  // Libraries String
		lds << "-Lbin/ ";
		for(const auto& lib : libraries) {
			if(lib.type != header_library) {
				ls << "-l" << lib.name.string() << ' ';
			}
		}

		for(const auto& lib_dir : pkg.library_dirs) { lds << "-L" << lib_dir.value << ' '; }
		for(const auto& lib : pkg.libraries) { ls << "-l" << lib.value << ' '; }

		// Convert all source file names into corresponding object file names.
		std::stringstream obj_files;
		for(const auto& source : pkg.sources) {
			obj_files << (pkg_build_dir / (source.value.stem().replace_extension(".o"))).string() << ' ';
		}

		_STD_BUILD_VERBOSE_OUTPUT("  Linking...\n");
		std::stringstream output;
		output << _STD_BUILD_COMPILER << ' ' << obj_files.str() << " -o " << (bin_dir / pkg.name) << ' ' << lds.str()
		       << ls.str();
		// _STD_BUILD_VERBOSE_OUTPUT('\n');
		if(command(output.str())) {
			throw executable_exception("There was an error during linking.", true);
		}

		// Update cache
		if(options().use_caching) {
			cache.update(pkg.include_dirs);
			cache.write_to_file(cache_path);
		}

		pkg.post();

		if(options().run_after_build) {
			_STD_BUILD_OUTPUT("Running...\n\n");
			auto exe = (bin_dir / pkg.name).replace_extension(".exe").make_preferred();
			if(fs::exists(exe)) {
				command(exe.string());
			} else {
				_STD_BUILD_OUTPUT("Failed to find executable.\n");
			}
		}
	}
} // namespace _STD_BUILD