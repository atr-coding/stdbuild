#pragma once

#include "base.h"
#include "options.h"
#include "threadpool.h"
#include "compiler.h"

namespace _STD_BUILD {

	struct _Library_Output {
		fs::path name;
		library_type type{ static_library };
	};

	using DependencyCollection = std::vector<package*>;

	void _add_packages(DependencyCollection& dep, package& project) {
		for(auto& pkg : project.pkgs) {
			dep.push_back(&pkg);
			_add_packages(dep, pkg);
		}
	};

	DependencyCollection _build_dependency_collection(package& main_project) {
		DependencyCollection dep;
		_add_packages(dep, main_project);

		// Remove duplicates.
		std::sort(dep.begin(), dep.end(), [](const package* a, const package* b) { return (*a < *b); });
		dep.erase(std::unique(dep.begin(), dep.end(), [](const package* a, const package* b) { return (*a == *b); }),
		          dep.end());

		return dep;
	}

	void create_binary(const Compiler& compiler, package& pkg, bool build_dependencies);

	void _build_dependencies(const Compiler& compiler, package& pkg) {
		DependencyCollection dep = _build_dependency_collection(pkg);
		// std::vector<_Library_Output> libraries;
		for(package* d : dep) {
			create_binary(compiler, *d, false);
			// libraries.push_back(create_binary(compiler, *d));

			if(d->type != executable && d->type != header_library) {
				pkg.libraries.add(d->name);
			}

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

		// return libraries;
	}

	// Transform the include and library directories in the package by prepending the
	// packages working directory to them.
	// This transforms the directories from local to absolute.
	inline void _transform_pkg_directories(package& pkg) noexcept {
		for(auto& id : pkg.include_dirs) { id.value = pkg.dir / id.value; }
		for(auto& ld : pkg.library_dirs) { ld.value = pkg.dir / ld.value; }
	}

	void create_binary(const Compiler& compiler, package& pkg, bool build_dependencies = false) {
		const auto bin_dir = options().bin_dir;
		const auto build_dir = options().build_dir;
		const auto package_build_dir = build_dir / pkg.name;

		pkg.pre();

		_transform_pkg_directories(pkg);

		// Verify that we have source files to work with.
		if(pkg.sources.size() == 0 && pkg.pre_built == false && pkg.type != header_library) {
			// TODO: Change this to binary_exeception
			throw executable_exception("No source files were given to compile for package: \"" + pkg.name + "\"");
		}

		// std::vector<_Library_Output> libraries;
		if(build_dependencies) {
			// libraries = _build_dependencies(compiler, pkg);
			_build_dependencies(compiler, pkg);
		}

		if(pkg.type == executable) {
			_STD_BUILD_OUTPUT("Building | Type: Executable     | Name: " << pkg.name << '\n');
		} else {
			_STD_BUILD_OUTPUT("Building | Type: "
			                  << (pkg.type == header_library ?
			                          "Header Library" :
			                          (pkg.type == static_library ? "Static Library" : "Shared Library"))
			                  << " | Name: " << pkg.name << '\n');
		}

		if(pkg.pre_built == false) {
			// Add debug flag
			if(debugging_enabled()) {
				pkg.flags += compiler.profile().debug_flag;
			}

			// Load Cache
			__cache::cache_storage cache;
			const auto cache_path = package_build_dir / "cache";
			path_list files_to_compile = __cache::load_cache(cache_path, cache, pkg);

			// Compile
			compile(compiler, package_build_dir, pkg, files_to_compile);

			// Link
			_STD_BUILD_VERBOSE_OUTPUT("  Linking...\n");
			if(pkg.type != executable) { // Library
				if(pkg.type != header_library) { // Static or Dynamic Library
					const std::string link_str = compiler.link(package_build_dir, bin_dir, pkg);
					if(command(link_str)) {
						throw library_exception("There was an error in creating the library: " + pkg.name, true);
					}
				}
			} else { // Exectuable
				const std::string link_str = compiler.link(package_build_dir, bin_dir, pkg);
				if(command(link_str)) {
					throw executable_exception("There was an error during linking.", true);
				}
			}

			// Update cache
			if(options().use_caching) {
				cache.update(pkg.include_dirs);
				cache.write_to_file(cache_path);
			}
		}

		pkg.post();

		if(options().run_after_build && pkg.type == executable) {
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