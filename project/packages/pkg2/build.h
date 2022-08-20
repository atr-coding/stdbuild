#include "../stdbuild"

struct PKG2 : std::build::package {
	PKG2(std::build::build_path _dir) {
		name = "pkg2";
		dir = _dir;
		include_dirs += ".";
		type = std::build::build_type::header_library;
	}
};