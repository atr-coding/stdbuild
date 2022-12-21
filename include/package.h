#pragma once

#include "base.h"
#include "list.h"
#include "version.h"

namespace _STD_BUILD {

	struct package;
	using package_list = list_base<package>;

	struct package {
		package() = default;
		explicit package(const std::string& _name) : name(_name) {}
		string_list flags, libraries;
		path_list include_dirs, library_dirs, sources;
		package_list pkgs;
		version ver{ 0, 0, 0 };
		std::string name{ "package" };
		path dir{ "." };
		library_type type{ static_library };
		bool pre_built{ false };

		virtual void pre(){};
		virtual void post(){};

		bool operator<(const package& other) const { return (name < other.name); }
		bool operator==(const package& other) const { return (name == other.name); }
	};

} // namespace _STD_BUILD