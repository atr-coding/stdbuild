#pragma once

#include "base.h"

namespace _STD_BUILD {
	struct _Options {
		fs::path build_dir{ "build/" };
		fs::path bin_dir{ "bin/" };
		std::string output_redirect_str{ " 2>build/error" }; //">build/output 2>build/error" };
		// std::string output_file{ "build/output" };
		std::string error_file{ "build/error" };
		bool debug{ false };

		void set(const std::string& opt, bool value) { _user_defined_opts[opt] = value; }
		bool get(const std::string& opt) { return _user_defined_opts[opt]; }

	private:
		std::map<std::string, bool> _user_defined_opts;
	};

	inline _Options& options() {
		static _Options opt;
		return opt;
	}

	inline void set_build_directory(fs::path path) {
		if(!fs::exists(path)) {
			fs::create_directory(path);
		}

		options().build_dir = path;
		// options().output_file = (path / "output").string();
		options().error_file = (path / "error").string();
		options().output_redirect_str = " 2>" + options().error_file;
		//">" + options().output_file + " 2>" + options().error_file;
	}

	inline void set_bin_directory(fs::path path) {
		if(!fs::exists(path)) {
			fs::create_directory(path);
		}

		options().bin_dir = path;
	}

	inline void set_option(const std::string& opt, bool value) { options().set(opt, value); }

	inline bool get_option(const std::string& opt) { return options().get(opt); }

	inline void enable_debugging() { options().debug = true; }

	inline void disable_debugging() { options().debug = false; }

	inline bool debugging_enabled() { return options().debug; }

	inline bool is_windows() {
#if defined(_WIN32) || defined(_WIN64)
		return true;
#else
		return false;
#endif
	}

	inline bool is_unix() {
#if defined(__GNUC__)
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
#if defined(__GNUC__)
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
} // namespace _STD_BUILD