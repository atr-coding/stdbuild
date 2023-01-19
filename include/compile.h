#pragma once

#include "base.h"
#include "command.h"
#include "package.h"
#include "threadpool.h"

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
		dep.erase(std::unique(dep.begin(), dep.end(), [](const auto* a, const auto* b) { return (*a == *b); }),
		          dep.end());

		return dep;
	}

	void compile(const fs::path& build_dir, const package& pkg, const path_list& sources) {
		if(sources.size() > 0) {
			auto& tpool = options().tpool;
			_STD_BUILD_VERBOSE_OUTPUT("  Compiling...\n");

			for(const auto& file : sources) {
				std::stringstream output;
				output << _STD_BUILD_COMPILER << " -c " << pkg.flags;
				for(const auto& i : pkg.include_dirs) { output << "-I" << i; }
				output << pkg.dir / file.value << " -o " << (build_dir / file.value.stem()).replace_extension(".o");

				// If the thread pool has been initialized then add this compilation to it.
				// If it hasn't, then just run it in this thread.
				CompileCommand cmd(output.str(), file.value.string());
				if(tpool.initialized()) {
					tpool.addTask(std::move(cmd));
				} else {
					cmd();
				}
			}

			// Wait for all the compilations to complete before continuing.
			tpool.wait();
		}
	}

	void compile(const fs::path& build_dir, const package& pkg) { compile(build_dir, pkg, pkg.sources); }

} // namespace _STD_BUILD