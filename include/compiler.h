#pragma once

#include <functional>
#include <string>
#include <sstream>

#include "base.h"
#include "package.h"
#include "list.h"

namespace _STD_BUILD {
	struct CompilerProfile {
		std::string name;
		std::string debug_flag;
		std::function<std::string(const fs::path&, const package&, const fs::path&)> compile_func;
		std::function<std::string(const fs::path&, const fs::path&, const package&)> link_func;
	};

	class Compiler {
		CompilerProfile m_profile;

	public:
		Compiler() = delete;
		Compiler(const CompilerProfile& profile) : m_profile(profile) {
			_STD_BUILD_VERBOSE_OUTPUT("Using compiler configuration for: " << m_profile.name << '\n');
		}
		const CompilerProfile& profile() const { return m_profile; }
		const std::string compile(const fs::path& pkg_build_dir,
		                          const package& pkg,
		                          const fs::path& source_file) const {
			return m_profile.compile_func(pkg_build_dir, pkg, source_file);
		}
		const std::string link(const fs::path& pkg_build_dir, const fs::path& bin_dir, const package& pkg) const {
			return m_profile.link_func(pkg_build_dir, bin_dir, pkg);
		}
	};

	namespace compilers {
		const CompilerProfile gcc = {
			"g++",
			"-g",
			[](const fs::path& pkg_build_dir, const package& pkg, const fs::path& source_file) -> std::string {
			    std::stringstream output;
			    output << "g++ -c " << pkg.flags;
			    for(const auto& i : pkg.include_dirs) { output << "-I" << i.value.lexically_normal() << ' '; }
			    output << pkg.dir / source_file << " -o "
			           << (pkg_build_dir / source_file.stem()).replace_extension(".o").lexically_normal();
			    return output.str();
			},
			[](const fs::path& pkg_build_dir, const fs::path& bin_dir, const package& pkg) -> std::string {
			    std::stringstream output;

			    // Convert all source file names into corresponding object file names.
			    std::stringstream obj_files;
			    for(const auto& source : pkg.sources) {
				    obj_files << (pkg_build_dir / (source.value.stem().replace_extension(".o"))).lexically_normal()
				              << ' ';
			    }

			    if(pkg.type == executable) {
				    std::stringstream lib_dirs; // Library directories
				    std::stringstream libs;     // Libraries
				    lib_dirs << "-L" << fs::path{"./bin/"}.lexically_normal() << ' ';

				    for(const auto& lib_dir : pkg.library_dirs) { lib_dirs << "-L" << lib_dir.value.lexically_normal() << ' '; }
				    for(const auto& lib : pkg.libraries) { libs << "-l" << lib.value << ' '; }

				    output << "g++ " << obj_files.str() << "-o " << (bin_dir / pkg.name).lexically_normal() << ' ' << lib_dirs.str()
				           << libs.str();
			    } else {
				    if(pkg.type == static_library) {
					    output << "ar rcs ";
				    } else if(pkg.type == shared_library) {
					    output << "g++ -shared -o ";
				    }

				    output << bin_dir / fs::path("lib" + pkg.name + (pkg.type == static_library ? ".a" : ".so")) << ' '
				           << obj_files.str();
			    }

			    return output.str();
			}
		};
	}

} // namespace _STD_BUILD
