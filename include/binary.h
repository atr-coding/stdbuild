#pragma once

#include "base.h"

namespace _STD_BUILD {

	inline bool create_executable(package& project) {
		// Verify that we have source files to work with.
		if(project.sources.size() == 0) {
			_STD_BUILD_OUTPUT("No source files.\n");
			_STD_BUILD_FAILURE_RETURN();
		}

		for(const auto& source : project.sources) {
			if(fs::exists(source.value) == false) {
				_STD_BUILD_OUTPUT("Source file: " << source.value.string() << " could not be found.\n");
				_STD_BUILD_FAILURE_RETURN();
			}
		}

		if(!_verify_bin_and_build_directories(project.name)) {
			return false;
		};
		const auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto project_build_dir = build_dir / project.name;

		// Transform include & library directories by prepending the project's working directory
		for(auto& id : project.include_dirs) {
			id = (project.dir / id.value);
		}

		for(auto& ld : project.library_dirs) {
			ld = (project.dir / ld.value);
		}

		// Load Cache
		_STD_BUILD_OUTPUT("Loading cache: ");
		const auto cache_path = build_dir / project.name / "cache";
		__cache::cache_storage cache;
		path_list files_to_compile;

		const auto [initialized, new_cache] = __cache::initialize_cache(cache_path, project.include_dirs, project.sources);
		if(initialized) {
			files_to_compile = project.sources;
			cache = new_cache;
		} else {
			if(!cache.load_from_file(cache_path)) {
				files_to_compile = project.sources;
				_STD_BUILD_OUTPUT("failed to load file.\n");
			} else {
				cache.test(project.sources);
				files_to_compile += cache.changes.added;
				files_to_compile += cache.changes.modified;
				_STD_BUILD_OUTPUT("complete [" << files_to_compile.size() + cache.changes.removed.size() << " changes]\n");
			}
		}

		// Add all dependency pointers to a vector and remove duplicates with the same name.
		auto dep = _build_dependency_vector(project);

		// Create the libraries and add them to a vector for future iteration
		std::vector<_Library_Output> libraries;
		if(dep.size() > 0) {
			_STD_BUILD_VERBOSE_OUTPUT("Building Dependencies: ");
			for([[maybe_unused]] const auto* p : dep) {
				_STD_BUILD_VERBOSE_OUTPUT(p->name << ' ');
			}
			_STD_BUILD_VERBOSE_OUTPUT('\n');
			for(package* d : dep) {
				if(auto lib = create_library(*d)) {
					libraries.push_back(lib.value());
				} else {
					_STD_BUILD_FAILURE_RETURN();
				}

				// Add this dependencies include/lib directories to our main project's include/lib directories
				for(const auto& inc_dir : d->include_dirs) {
					if(inc_dir.access_level == access::all) {
						project.include_dirs.add(inc_dir);
					}
				}

				for(const auto& lib_dir : d->library_dirs) {
					if(lib_dir.access_level == access::all) {
						project.library_dirs.add(lib_dir);
					}
				}
			}
		}

		_STD_BUILD_OUTPUT(project.name << " - executable");
		_STD_BUILD_VERBOSE_OUTPUT('\n');

		// Add debug flag
		if(debugging_enabled()) {
			project.flags += "-g";
		}

		auto comp = compile(project_build_dir, project.dir, project.flags, project.include_dirs, files_to_compile);
		if(!comp) {
			_STD_BUILD_FAILURE_RETURN();
		}

		std::stringstream lds; // Library Directory String
		std::stringstream ls;  // Libraries String
		for(const auto& lib : libraries) {
			if(lib.type != header_library) {
				lds << "-L" << lib.location.string() << ' ';
				ls << "-l" << lib.name.string() << ' ';
			}
		}

		for(const auto& lib_dir : project.library_dirs) {
			ls << "-L" << lib_dir.value << ' ';
		}

		for(const auto& lib : project.libraries) {
			ls << "-l" << lib.value << ' ';
		}

		// Convert all source file names into corresponding object file names.
		std::stringstream obj_files;
		for(const auto& source : project.sources) {
			obj_files << (project_build_dir / (source.value.stem().replace_extension(".o"))).string() << ' ';
		}

		_STD_BUILD_VERBOSE_OUTPUT("Linking...");
		std::stringstream output;
		output << _STD_BUILD_COMPILER << ' ' << obj_files.str() << " -o " << (bin_dir / project.name) << ' ' << lds.str() << ls.str();
		_STD_BUILD_VERBOSE_OUTPUT('\n');
		if(command(output.str())) {
			_STD_BUILD_OUTPUT("\nThere was an error during linking:\n");
			_print_error_log();
			_STD_BUILD_FAILURE_RETURN();
		} else {
			_STD_BUILD_VERBOSE_OUTPUT("output");
			_STD_BUILD_OUTPUT(" - " << (bin_dir / project.name).string() << '\n');
		}

		// Update cache
		// std::cout << "Before updating: \n\n" << cache << "\n\n";
		cache.update(project.include_dirs);
		// std::cout << "After updating: \n\n" << cache << "\n\n";
		cache.write_to_file(cache_path);

#ifdef _STD_BUILD_RUN
		_STD_BUILD_OUTPUT("Running...\n\n");
		auto exe = (bin_dir / project.name).replace_extension(".exe").make_preferred();
		if(fs::exists(exe)) {
			command(exe.string());
		} else {
			_STD_BUILD_OUTPUT("Failed to find executable.\n");
		}
#endif
		return true;
	}

} // namespace _STD_BUILD