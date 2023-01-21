#pragma once

#include "base.h"
#include "threadpool.h"

namespace _STD_BUILD {

	struct _Options {
		fs::path build_dir{ "build/" };
		fs::path bin_dir{ "bin/" };
		std::string output_redirect_str{ " 2>build/error" }; //">build/output 2>build/error" };
		// std::string output_file{ "build/output" };
		std::string error_file{ "build/error" };
		ThreadPool<CompileCommand> tpool;
		fs::path package_directory{ "packages/" };
		bool debug{ false };
		bool use_caching{ true };
		bool use_verbose_output{ false };
		bool run_after_build{ false };

		void set(const std::string& opt, bool value) { _user_defined_opts[opt] = value; }
		bool get(const std::string& opt) { return _user_defined_opts[opt]; }

	private:
		std::map<std::string, bool> _user_defined_opts;
	};

	inline _Options& options() {
		static _Options opt;
		return opt;
	}

	inline void set_build_directory(const fs::path& path) {
		options().build_dir = path;
		// options().output_file = (path / "output").string();
		options().error_file = (path / "error").string();
		options().output_redirect_str = " 2>" + options().error_file;
		//">" + options().output_file + " 2>" + options().error_file;
	}

	inline void set_bin_directory(const fs::path& path) { options().bin_dir = path; }

	inline void set_package_directory(const fs::path& path) { options().package_directory = path; }

	inline void set_option(const std::string& opt, bool value) {
		_STD_BUILD_VERBOSE_OUTPUT("Option: [" << opt << ", " << std::boolalpha << value << "]\n");
		options().set(opt, value);
	}

	inline bool get_option(const std::string& opt) { return options().get(opt); }

	inline void enable_debugging() {
		_STD_BUILD_VERBOSE_OUTPUT("Debugging enabled.\n");
		options().debug = true;
	}

	inline void disable_debugging() {
		_STD_BUILD_VERBOSE_OUTPUT("Debugging disabled.\n");
		options().debug = false;
	}

	inline bool debugging_enabled() { return options().debug; }

	inline bool is_windows() {
#if defined(_WIN32) || defined(_WIN64)
		return true;
#else
		return false;
#endif
	}

	inline bool is_unix() {
#if defined(__GNUC__) || defined(__GNUG__)
		return true;
#else
		return false;
#endif
	}

	inline bool is_32_bit() {
#if defined(_WIN32) || (defined(__GNUC__) && !defined(__x86_64__) && !defined(__ppc64__))
		return true;
#else
		return false;
#endif
	}

	inline bool is_64_bit() {
#if defined(_WIN64) || (defined(__GNUC__) && (defined(__x86_64__) || !defined(__ppc64__)))
		return true;
#else
		return false;
#endif
	}

	inline bool is_msvc() {
#if defined(_MSC_VER)
		return true;
#else
		return false;
#endif
	}

	inline bool is_gcc() {
#if defined(__GNUC__) || defined(__GNUG__)
		return true;
#else
		return false;
#endif
	}

	inline bool is_clang() {
#if defined(__clang__)
		return true;
#else
		return false;
#endif
	}

	inline void run_after_build(bool value) { options().run_after_build = value; }

	inline void enable_multithreading(uint16_t thread_count) {
		_STD_BUILD_VERBOSE_OUTPUT("Multithreading enabled with " << thread_count << " threads.\n");
		options().tpool.init(thread_count);
	}

	inline void enable_caching() {
		_STD_BUILD_VERBOSE_OUTPUT("Caching enabled.\n");
		options().use_caching = true;
	}

	inline void disable_caching() {
		_STD_BUILD_VERBOSE_OUTPUT("Caching disabled.\n");
		options().use_caching = false;
	}

	inline void enable_verbose_output() { options().use_verbose_output = true; }

	inline void disable_verbose_output() { options().use_verbose_output = false; }
} // namespace _STD_BUILD