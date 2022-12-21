#pragma once

#include "base.h"
//#include "options.h"

namespace _STD_BUILD {
	void _print_error_log() {
		std::ifstream error(options().error_file);
		if(error) {
			std::cout << error.rdbuf() << "\n\n";
			error.close();
		}
	}
} // namespace _STD_BUILD