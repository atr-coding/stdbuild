#pragma once

#include <list>

#include "base.h"

namespace _STD_BUILD {

	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::list<T>& list) {
		for (const auto& item : list) {
			stream << item << ' ';
		}
		return stream;
	}

	using string_list = std::list<std::string>;
	// using path_list = std::list<fs::path>;

} // namespace _STD_BUILD