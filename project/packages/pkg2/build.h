#include "../../../stdbuild"

struct PKG2 : std::build::package {
	PKG2(std::build::path _dir) {
		name = "pkg2";
		dir = _dir;
		include_dirs += ".";
		type = std::build::header_library;
	}
};