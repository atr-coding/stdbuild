#include "../../../stdbuild"

struct PKG2 : stdbuild::package {
	PKG2(stdbuild::path _dir) {
		name = "pkg2";
		dir = _dir;
		include_dirs += ".";
		type = stdbuild::header_library;
	}
};