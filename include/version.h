#pragma once

#include "base.h"

namespace _STD_BUILD {
	struct version {
		version() = default;
		version(uint32_t major, uint32_t minor, uint32_t patch) : m_major(major), m_minor(minor), m_patch(patch) {}

		uint32_t major() { return m_major; }
		uint32_t major() const { return m_major; }
		uint32_t minor() { return m_minor; }
		uint32_t minor() const { return m_minor; }
		uint32_t patch() { return m_patch; }
		uint32_t patch() const { return m_patch; }

		bool operator>(const version& other) const { return (m_major > other.m_major || m_minor > other.m_minor || m_patch > other.m_patch); }

		bool operator<(const version& other) const { return (m_major < other.m_major || m_minor < other.m_minor || m_patch < other.m_patch); }

		bool operator==(const version& other) const { return (m_major == other.m_major && m_minor == other.m_minor && m_patch == other.m_patch); }

		bool operator!=(const version& other) const { return !(*this == other); }

		bool operator>=(const version& other) const { return (*this > other || *this == other); }

		bool operator<=(const version& other) const { return (*this < other || *this == other); }

		std::string string() {
			std::stringstream ss;
			ss << m_major << '.' << m_minor << '.' << m_patch;
			return ss.str();
		}

	private:
		uint32_t m_major{ 0 }, m_minor{ 0 }, m_patch{ 0 };
	};
} // namespace stdbuild