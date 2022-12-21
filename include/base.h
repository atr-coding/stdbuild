#pragma once

#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <type_traits>
#include <utility>
#include <memory>
#include <exception>

#define _STD_BUILD stdbuild
#define _STD_BUILD_OUTPUT(str) std::cout << str;

#ifdef _STD_BUILD_VERBOSE
#define _STD_BUILD_VERBOSE_OUTPUT(str) std::cout << str
#else
#define _STD_BUILD_VERBOSE_OUTPUT(str)
#endif

#if defined(__clang__)
#define _STD_BUILD_COMPILER "clang++"
#elif defined(__GNUC__) || defined(__GNUG__)
#define _STD_BUILD_COMPILER "g++"
#elif defined(_MSC_VER)
#define _STD_BUILD_COMPILER "cl"
#else
#error A supported compiler (msvc, g++, clang) must be used.
#endif

#define _STD_BUILD_FAILURE() std::exit(1);

// TODO: Disable cache flag. (maybe a option)

#if __cplusplus < 201703L
#error C++17 is needed for filesystem support.
#endif

namespace _STD_BUILD {

	namespace fs = std::filesystem;
	using path = fs::path;

	enum class library_type : uint8_t { STATIC, SHARED, HEADER, EXECUTABLE };

	inline const library_type static_library = library_type::STATIC;
	inline const library_type shared_library = library_type::SHARED;
	inline const library_type header_library = library_type::HEADER;
	inline const library_type executable = library_type::EXECUTABLE;

	const std::vector<std::string> std_header_files = {
		"bitset",
		"chrono",
		"compare",
		"csetjmp",
		"csignal",
		"cstdarg",
		"cstddef",
		"cstdlib",
		"ctime",
		"expected",
		"functional",
		"initializer_list",
		"optional",
		"source_location",
		"tuple",
		"type_traits",
		"typeindex",
		"typeinfo",
		"utility",
		"variant",
		"version",
		"memory",
		"memory_resource",
		"new",
		"scoped_allocator",
		"cfloat",
		"cinttypes",
		"climits",
		"cstdint",
		"limits",
		"stdfloat",
		"cassert",
		"cerrno",
		"exception",
		"stacktrace",
		"stdexcept",
		"system_error",
		"cctype",
		"charconv",
		"cstring",
		"cuchar",
		"cwchar",
		"cwctype",
		"format",
		"string",
		"string_view",
		"array",
		"deque",
		"flat_map",
		"flat_set",
		"forward_list",
		"list",
		"map",
		"mdspan",
		"queue",
		"set",
		"span",
		"stack",
		"unordered_map",
		"unordered_set",
		"vector",
		"iterator",
		"generator",
		"ranges",
		"algorithm",
		"execution",
		"bit",
		"cfenv",
		"cmath",
		"complex",
		"numbers",
		"numeric",
		"random",
		"ratio",
		"valarray",
		"clocale",
		"codecvt",
		"locale",
		"cstdio",
		"fstream",
		"iomanip",
		"ios",
		"iosfwd",
		"iostream",
		"istream",
		"ostream",
		"print",
		"spanstream",
		"sstream",
		"streambuf",
		"strstream",
		"syncstream",
		"filesystem",
		"regex",
		"atomic",
		"barrier",
		"condition_variable",
		"future",
		"latch",
		"mutex",
		"semaphore",
		"shared_mutex",
		"stop_token",
		"thread",
		"ctype.h",
		"errno.h",
		"fenv.h",
		"float.h",
		"inttypes.h",
		"limits.h",
		"locale.h",
		"math.h",
		"setjmp.h",
		"signal.h",
		"stdarg.h",
		"stddef.h",
		"stdint.h",
		"stdio.h",
		"stdlib.h",
		"string.h",
		"time.h",
		"uchar.h",
		"wchar.h",
		"wctype.h",
		"stdatomic.h",
		"ccomplex",
		"complex.h",
		"ctgmath",
		"tgmath.h",
		"ccomplex",
		"complex.h",
		"ctgmath",
		"tgmath.h",
		"assert.h",
		"crtdbg.h",
		"Windows.h",
	};

	/// Exceptions ///
	void _print_error_log();

	struct build_exception : std::exception {
		build_exception(const std::string& msg) : m_msg(msg) {}
		const char* what() const noexcept { return m_msg.c_str(); }

	private:
		std::string m_msg;
	};

	struct compile_exception : std::exception {
		compile_exception(const std::string& msg, bool print_error_log = false) : m_msg(msg) {
			if(print_error_log) {
				_print_error_log();
			}
		}
		const char* what() const noexcept { return m_msg.c_str(); }

	private:
		std::string m_msg;
	};

	struct library_exception : std::exception {
		library_exception(const std::string& msg, bool print_error_log = false) : m_msg(msg) {
			if(print_error_log) {
				_print_error_log();
			}
		}
		const char* what() const noexcept { return m_msg.c_str(); }

	private:
		std::string m_msg;
	};

	struct executable_exception : std::exception {
		executable_exception(const std::string& msg, bool print_error_log = false) : m_msg(msg) {
			if(print_error_log) {
				_print_error_log();
			}
		}
		const char* what() const noexcept { return m_msg.c_str(); }

	private:
		std::string m_msg;
	};

	struct cache_exception: std::exception {
		cache_exception(const std::string& msg) : m_msg(msg) {}
		const char* what() const noexcept { return m_msg.c_str(); }

	private:
		std::string m_msg;
	};

	/// Exceptions end ///
	
	bool _verify_bin_and_build_directories(const fs::path& bin_dir, const fs::path& build_dir, const std::string& project_name) {
		const auto& bin = bin_dir;
		const auto& build = build_dir / project_name;
		// Create the desired bin directory if it doesn't exist already.
		if(!fs::exists(bin)) {
			try {
				fs::create_directories(bin);
			} catch(std::exception& e) { throw build_exception("Failed to create bin directory " + bin.string()); }
		}

		// Create the desired build directory if it doesn't exist already.
		if(!fs::exists(build)) {
			try {
				fs::create_directories(build);
			} catch(std::exception& e) { throw build_exception("Failed to create build directory " + build.string()); }
		}

		return true;
	}

} // namespace _STD_BUILD