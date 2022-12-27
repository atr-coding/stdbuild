#pragma once

#include "base.h"
#include "options.h"
#include "list.h"

namespace _STD_BUILD {
	struct _pkg_url {
		std::string link;
		_pkg_url() = default;
		explicit _pkg_url(const std::string& _link) : link(_link) {}
	};

	struct _pkg_branch {
		std::string name;
		_pkg_branch() = default;
		explicit _pkg_branch(const std::string& _name) : name(_name) {}
	};

	struct _pkg_commit {
		std::string id;
		_pkg_commit() = default;
		explicit _pkg_commit(const std::string& _id) : id(_id) {}
	};

	struct external_dependency;
	using external_dependency_list = list_base<external_dependency>;

	struct external_dependency {
		std::string m_name;
		_pkg_url m_url;
		_pkg_branch m_branch;
		_pkg_commit m_commit;
		bool m_keep_up_to_date{ false };

		external_dependency() = delete;
		external_dependency(const std::string name, _pkg_url _url, bool keep_up_to_date = false)
		    : m_name(name), m_url(_url), m_keep_up_to_date(keep_up_to_date) {}
		external_dependency(const std::string name, _pkg_url _url, _pkg_branch _branch, bool keep_up_to_date = false)
		    : m_name(name), m_url(_url), m_branch(_branch), m_keep_up_to_date(keep_up_to_date) {}
		external_dependency(const std::string name, _pkg_url _url, _pkg_commit _commit, bool keep_up_to_date = false)
		    : m_name(name), m_url(_url), m_commit(_commit), m_keep_up_to_date(keep_up_to_date) {}
		external_dependency(const std::string name, _pkg_url _url, _pkg_branch _branch, _pkg_commit _commit, bool keep_up_to_date = false)
		    : m_name(name), m_url(_url), m_branch(_branch), m_commit(_commit), m_keep_up_to_date(keep_up_to_date) {}

		const std::string command_string() const {
			const auto& package_directory = options().package_directory;

			if(m_url.link.empty()) {
				_STD_BUILD_OUTPUT("  URL cannot be empty.\n");
				std::exit(1);
			}

			std::stringstream ss;

			ss << "cd " << package_directory.string() << " && git clone " << m_url.link << ' ' << m_name;

			if(!m_branch.name.empty()) {
				ss << " -b " << m_branch.name;
			}

			ss << " -q 2>packages-error.log";

			if(!m_commit.id.empty()) {
				ss << " && cd " << m_name << " && git -q reset --hard " << m_commit.id;
			}

			return ss.str();
		}
	};
}