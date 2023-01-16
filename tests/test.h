#pragma once
#include <iostream>
#include <string>
#include <cstring>

void _print_error_msg(const std::string& msg, const char* file, int line, const char* func) noexcept {
	// std::cerr << file << ':' << line << " [" << func << "] \x1B[31m" << msg << "\033[0m\n";
	// std::cerr << "\x1B[31m" << msg << "\033[0m [" << func << "][" << file << ':' << line << "]\n";
	std::cerr << "[" << func << "][" << file << ':' << line << "] \x1B[31m" << msg << "\033[0m\n";
}

#define print_error_msg(msg) _print_error_msg(msg, __FILE__, __LINE__, __FUNCTION__);

class test_monad {
	bool m_condition{ false };
public:
	constexpr test_monad() = delete;

	constexpr test_monad(bool condition) noexcept {
		m_condition = condition;
	}

	template<typename Func>
	constexpr test_monad& on_success(Func func) noexcept {
		if (m_condition) {
			func();
		}
		return *this;
	}

	template<typename Func>
	constexpr test_monad& on_fail(Func func) noexcept {
		if (!m_condition) {
			func();
		}
		return *this;
	}
};

constexpr test_monad _test(bool condition) {
	return { condition };
}

#define test(condition, msg) if(!(condition) && std::strlen(msg) > 0) { print_error_msg(msg); } _test(condition)