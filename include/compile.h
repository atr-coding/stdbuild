#pragma once

#include "base.h"
#include "command.h"
#include "package.h"
#include "threadpool.h"
#include "compiler.h"

namespace _STD_BUILD {

	////////////// Compilation //////////////

	void compile(const Compiler& compiler, const fs::path& build_dir, const package& pkg, const path_list& sources) {
		if(sources.size() > 0) {
			auto& tpool = options().tpool;
			_STD_BUILD_VERBOSE_OUTPUT("  Compiling...\n");

			for(const auto& file : sources) {
				std::string compile_str = compiler.compile(build_dir, pkg, file.value);
				
				// If the thread pool has been initialized then add this compilation to it.
				// If it hasn't, then just run it in this thread.
				// CompileCommand cmd(output.str(), file.value.string());
				CompileCommand cmd(compile_str, file.value.string());
				if(tpool.initialized()) {
					tpool.addTask(std::move(cmd));
				} else {
					cmd.execute();
				}
			}

			// Wait for all the compilations to complete before continuing.
			tpool.wait();
		}
	}

	void compile(const Compiler& compiler, const fs::path& build_dir, const package& pkg) {
		compile(compiler, build_dir, pkg, pkg.sources);
	}

} // namespace _STD_BUILD