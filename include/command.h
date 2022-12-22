#pragma once

#include "base.h"
#include "options.h"

namespace _STD_BUILD {
	int command(const std::string& cmd) {
		_STD_BUILD_VERBOSE_OUTPUT("  Command: " << cmd << '\n');
		return std::system((cmd /*+ options().output_redirect_str */).c_str());
	}
}