#pragma once

#include "base.h"
#include "options.h"
#include "command.h"
#include "error.h"
#include "cache.h"
#include "compile.h"
#include "link.h"
#include "package.h"
#include "version.h"
#include "link.h"
#include "binary.h"
#include "dependency.h"
#include "package_manager.h"

namespace _STD_BUILD {
	// This function acts as an entry point where we can put anything that we want to run
	// before any build regardless of type.
	void _init(const package& pkg) {
		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);
		_get_external_dependencies(pkg);
	}

	void create(package& pkg) {
		_init(pkg);
		Compiler compiler;
		if(is_clang()) {
			compiler.set_profile(compilers::clang);
		} else if(is_gcc()) {
			compiler.set_profile(compilers::gcc);
		} else if(is_windows()) {
			compiler.set_profile(compilers::msvc);
		} else {
			throw build_exception("A supported compiler (msvc, g++, clang) must be used.");
		}
		create_binary(compiler, pkg, true);
	}

	template<typename T, typename... Args>
	void create(Args... args) {
		T pkg(std::forward<Args>(args)...);
		create(pkg);
	}
}