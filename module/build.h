#include "../stdbuild"

namespace module {
	inline std::build::project build() {
		using namespace std::build;
		std::build::project build("module", type::static_library);
		set_directory(build, "module");
		add_flags(build, "-std=c++20");
		add_source_files(build, "test.cpp");
		add_include_directories(build, ".");
		return std::move(build);
	}
}