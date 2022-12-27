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
#include "library.h"
#include "executable.h"
#include "dependency.h"
#include "package_manager.h"

namespace _STD_BUILD {
	// This function acts as an entry point where we can put anything that we want to run
	// before any build regardless of type.
	void _init(const package& pkg) {
		// Check if our bin/build directories exist, and if not, create them.
		// Throws if the directories could not be created.
		_verify_bin_and_build_directories(options().bin_dir, options().build_dir, pkg.name);

		_get_external_dependencies(pkg);
	}

	void create(package& pkg) {
		_init(pkg);
		if (pkg.type == library_type::EXECUTABLE) {
			create_executable(pkg);
		} else {
			create_library(pkg);
		}
	}

	template<typename T, typename... Args>
	void create(Args... args) {
		T pkg(std::forward<Args>(args)...);
		_init(pkg);
		if(pkg.type == library_type::EXECUTABLE) {
			create_executable(pkg);
		} else {
			create_library(pkg);
		}
	}
}