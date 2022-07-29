#include "../stdbuild"

namespace module {
	inline std::build::BuildInfo build() {
		std::build::BuildInfo build("module", std::build::Type::static_library);
		build.dir = "module";
		build.flags.add("-std=c++20");
		build.files.add("test.cpp");
		build.include_dirs.add(".");
		return std::move(build);
	}
}