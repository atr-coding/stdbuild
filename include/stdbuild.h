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

namespace _STD_BUILD {
	void create(package& pkg) {
		if (pkg.type == library_type::EXECUTABLE) {
			create_executable(pkg);
		} else {
			create_library(pkg);
		}
	}

	template<typename T>
	void create() {
		T pkg;
		if(pkg.type == library_type::EXECUTABLE) {
			create_executable(pkg);
		} else {
			create_library(pkg);
		}
	}
}