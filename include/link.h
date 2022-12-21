#pragma once

#include "base.h"

namespace _STD_BUILD {
	/*bool link(const std::string& binary_name,
	      const fs::path& build_dir,
	      const path_list& obj_files,
	      const path_list& library_dirs,
	      const string_list& libraries) {
	_STD_BUILD_VERBOSE_OUTPUT("Linking...");
	const std::string binary_path = (options().bin_dir / binary_name).string();
	std::stringstream output;
	output << _STD_BUILD_COMPILER << ' ' << obj_files.value() << " -o " << binary_path << ' ' << lids.str() << ls.str();
	_STD_BUILD_VERBOSE_OUTPUT('\n');
	if(command(output.str())) {
	    _STD_BUILD_OUTPUT("\nThere was an error during linking:\n");
	    _print_error_log();
	    _STD_BUILD_FAILURE_RETURN();
	} else {
	    _STD_BUILD_VERBOSE_OUTPUT("output");
	    _STD_BUILD_OUTPUT(" - " << binary_path << '\n');
	}
}*/
}