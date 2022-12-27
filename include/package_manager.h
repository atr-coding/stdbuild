#pragma once

#include "base.h"
#include "options.h"
#include "package.h"
#include "dependency.h"

namespace _STD_BUILD {

	void _get_external_dependencies(const package& pkg) {
		const auto& package_directory = options().package_directory;

		// Create our desired package directory if it doesn't exist.
		if(!fs::exists(package_directory)) {
			fs::create_directories(package_directory);
		}

		std::vector<const external_dependency*> dependencies;

		// Recursively loop through all the packages and collect pointers to their external depedencies.
		const std::function<void(const package&)> collect_dependencies = [&](const package& _pkg_outer) {
			for(const auto& _pkg_inner : _pkg_outer.pkgs) { collect_dependencies(_pkg_inner); }
			for(const auto& dep : _pkg_outer.external_dependencies) { dependencies.push_back(&dep); }
		};
		collect_dependencies(pkg);

		// Remove duplicate dependencies.
		std::sort(dependencies.begin(), dependencies.end(),
		          [](const external_dependency* a, const external_dependency* b) { return a->m_name < b->m_name; });
		std::unique(dependencies.begin(), dependencies.end(),
		            [](const external_dependency* a, const external_dependency* b) { return a->m_name == b->m_name; });

		// Iterate through every external dependency.
		// If the directory already exists, then we assume it's been downloaded and
		// check to see if it needs to be updated.
		// If it needs to be updated, we cd into the packages directory and run
		// a git pull request and check the result.
		for(const auto* dep : dependencies) {
			const auto pkg_dir{ package_directory / dep->m_name };
			if(!fs::exists(pkg_dir)) {
				_STD_BUILD_OUTPUT("Downloading dependency: " << dep->m_name << '\n');

				const auto str = dep->command_string();

				_STD_BUILD_OUTPUT("  Url: " << dep->m_url.link << '\n');
				_STD_BUILD_OUTPUT("  Branch: " << (dep->m_branch.name.empty() ? "Default" : dep->m_branch.name) << '\n');
				_STD_BUILD_OUTPUT("  Commit: " << (dep->m_commit.id.empty() ? "Latest" : dep->m_commit.id) << '\n');
				_STD_BUILD_OUTPUT("  Status: ");
				int ret = std::system(str.c_str());
				if(ret != 0 || !fs::exists(pkg_dir)) {
					_STD_BUILD_OUTPUT("Failed to clone dependency.");

					// Print the error log
					std::ifstream error_log(package_directory / "packages-error.log");
					if(error_log.is_open()) {
						_STD_BUILD_OUTPUT("\n    packages-error.log:\n");
						std::string line;
						while(std::getline(error_log, line)) { _STD_BUILD_OUTPUT("      " << line << '\n'); }
						error_log.close();
					} else {
						_STD_BUILD_OUTPUT(" See " << package_directory.string() << "/packages-error.log for details.\n");
					}
					std::exit(1);
				}
				_STD_BUILD_OUTPUT("Success\n");
			} else {
				// If the commit id is not empty, then we assume that we want a specific commit and
				// that we should not ever update.
				if(dep->m_keep_up_to_date && dep->m_commit.id.empty()) {
					std::stringstream ss;
					ss << "cd " << pkg_dir.string() << " && git pull -q 2>error.log";
					_STD_BUILD_OUTPUT("Updating dependency: " << dep->m_name << '\n');
					_STD_BUILD_OUTPUT("  Status: ");
					int ret = std::system(ss.str().c_str());
					if(ret != 0 || !fs::exists(pkg_dir)) {
						_STD_BUILD_OUTPUT("Failed to update dependency.");

						// Print the error log
						std::ifstream error_log(pkg_dir / "error.log");
						if(error_log.is_open()) {
							_STD_BUILD_OUTPUT("\n    error.log:\n");
							std::string line;
							while(std::getline(error_log, line)) { _STD_BUILD_OUTPUT("      " << line << '\n'); }
							error_log.close();
						} else {
							_STD_BUILD_OUTPUT(" See " << pkg_dir.string() << "/error.log for details.\n");
						}
						std::exit(1);
					}
					_STD_BUILD_OUTPUT("Success\n");
				}
			}
		}
	}

} // namespace _STD_BUILD