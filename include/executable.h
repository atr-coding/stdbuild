#pragma once

#include "base.h"
#include "options.h"

namespace _STD_BUILD {

	inline void create_executable(package& pkg) {
		// Verify that we have source files to work with.
		if(pkg.sources.size() == 0) {
			throw executable_exception("No source files to compile.");
		}

		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);

		const auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto pkg_build_dir = build_dir / pkg.name;

		pkg.pre();

		// Transform include & library directories by prepending the pkg's working directory
		for(auto& id : pkg.include_dirs) {
			id = (pkg.dir / id.value);
		}

		for(auto& ld : pkg.library_dirs) {
			ld = (pkg.dir / ld.value);
		}

		// Load Cache
		_STD_BUILD_OUTPUT("Loading cache: ");
		const auto cache_path = build_dir / pkg.name / "cache";
		__cache::cache_storage cache;
		path_list files_to_compile;

		const auto [initialized, new_cache] = __cache::initialize_cache(cache_path, pkg.include_dirs, pkg.sources);
		if(initialized) {
			files_to_compile = pkg.sources;
			cache = new_cache;
		} else {
			if(!cache.load_from_file(cache_path)) {
				files_to_compile = pkg.sources;
				_STD_BUILD_OUTPUT("failed to load file.\n");
			} else {
				cache.test(pkg.sources);
				files_to_compile += cache.changes.added;
				files_to_compile += cache.changes.modified;
				_STD_BUILD_OUTPUT("complete [" << files_to_compile.size() + cache.changes.removed.size() << " changes]\n");
			}
		}

		// Add all dependency pointers to a vector and remove duplicates with the same name.
		auto dep = _build_dependency_vector(pkg);

		// Create the libraries and add them to a vector for future iteration
		std::vector<_Library_Output> libraries;
		if(dep.size() > 0) {
			_STD_BUILD_VERBOSE_OUTPUT("Building Dependencies: ");
			for([[maybe_unused]] const auto* p : dep) {
				_STD_BUILD_VERBOSE_OUTPUT(p->name << ' ');
			}
			_STD_BUILD_VERBOSE_OUTPUT('\n');
			for(package* d : dep) {
				libraries.push_back(create_library(*d));

				// Add this dependencies include/lib directories to our main pkg's include/lib directories
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
			}
		}

		_STD_BUILD_OUTPUT(pkg.name << " - executable");
		_STD_BUILD_VERBOSE_OUTPUT('\n');

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
				// lds << "-L" << lib.location.string() << ' ';
				ls << "-l" << lib.name.string() << ' ';
			}
		}

		for(const auto& lib_dir : pkg.library_dirs) {
			ls << "-L" << lib_dir.value << ' ';
		}

		for(const auto& lib : pkg.libraries) {
			ls << "-l" << lib.value << ' ';
		}

		// Convert all source file names into corresponding object file names.
		std::stringstream obj_files;
		for(const auto& source : pkg.sources) {
			obj_files << (pkg_build_dir / (source.value.stem().replace_extension(".o"))).string() << ' ';
		}

		_STD_BUILD_VERBOSE_OUTPUT("Linking...");
		std::stringstream output;
		output << _STD_BUILD_COMPILER << ' ' << obj_files.str() << " -o " << (bin_dir / pkg.name) << ' ' << lds.str() << ls.str();
		_STD_BUILD_VERBOSE_OUTPUT('\n');
		if(command(output.str())) {
			throw executable_exception("There was an error during linking.", true);
		} else {
			_STD_BUILD_VERBOSE_OUTPUT("output");
			_STD_BUILD_OUTPUT(" - " << (bin_dir / pkg.name).string() << '\n');
		}

		// Update cache
		cache.update(pkg.include_dirs);
		cache.write_to_file(cache_path);

		pkg.post();

		if(running_after_build()) {
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