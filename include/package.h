#pragma once

#include "base.h"
#include "list.h"
#include "version.h"
#include "dependency.h"

namespace _STD_BUILD {

	struct package;
	using package_list = list_base<package>;

	struct package {
		package() noexcept = default;
		explicit package(const std::string& _name) noexcept : name(_name) {}
		virtual ~package() noexcept {};
		string_list flags, libraries;
		path_list include_dirs, library_dirs, sources;
		package_list pkgs;
		version ver{ 0, 0, 0 };
		std::string name{ "package" };
		path dir{ "." };
		library_type type{ executable };
		bool pre_built{ false };
		external_dependency_list external_dependencies;

		using url = _pkg_url;
		using branch = _pkg_branch;
		using commit = _pkg_commit;

		virtual void pre(){};
		virtual void post(){};

		bool operator<(const package& other) const noexcept { return (name < other.name); }
		bool operator==(const package& other) const noexcept { return (name == other.name); }
	};

} // namespace _STD_BUILD