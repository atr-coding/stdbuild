#pragma once

#include "base.h"
#include "command.h"
#include "package.h"
#include "compiler.h"

namespace _STD_BUILD {

	////////////// Compilation //////////////

	void compile(const Compiler& compiler, const fs::path& build_dir, const package& pkg, const std::list<fs::path>& sources) {
		if(sources.size() > 0) {
			_STD_BUILD_VERBOSE_OUTPUT("  Compiling...\n");

			std::vector<std::future<void>> compilations;

			for(const auto& file : sources) {
				std::string compile_str = compiler.compile(build_dir, pkg, file);

				compilations.emplace_back(std::async(std::launch::async, [&](const std::string& cmd, const std::string& file){
					if(command(cmd)){
						throw compile_exception("There was an error while compiling: " + file, true);
					}
				}, compile_str, file.string()));
			}

			// Wait for all tasks to finish.
			// The get method will propagate any compile_exceptions.
			for(auto& c : compilations) {
				c.get();
			}
		}
	}

	void compile(const Compiler& compiler, const fs::path& build_dir, const package& pkg) {
		compile(compiler, build_dir, pkg, pkg.sources);
	}

} // namespace _STD_BUILD