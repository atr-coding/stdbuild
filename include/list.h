#pragma once

#include "base.h"

namespace _STD_BUILD {

	// all = public, parent = interface, self = private
	enum class access : uint8_t { all, parent, self };

	template <typename Type>
	class list_base {
	private:
		template <typename _Type, typename Stream, typename = void>
		struct is_streamable : std::false_type {};

		template <typename _Type, typename Stream>
		struct is_streamable<_Type, Stream, std::void_t<decltype(std::declval<Stream&>() << std::declval<_Type>())>> : std::true_type {};

	public:
		list_base() = default;
		list_base(const Type&) = delete;
		list_base(const list_base<Type>& list) : m_items(list.m_items) {}
		list_base(list_base<Type>&& list) noexcept {
			if(this != &list) {
				m_items = std::move(list.m_items);
			}
		}
		list_base(const std::initializer_list<Type>& elements) { m_items.insert(m_items.end(), elements.begin(), elements.end()); }

		/// Misc Functions ///

		std::vector<Type>& get() { return m_items; }
		const std::vector<Type>& get() const { return m_items; }
		std::size_t size() const { return m_items.size(); }
		std::size_t size() { return m_items.size(); }
		void add(const Type& item) { m_items.push_back(item); }

		// TODO: improve this
		template <typename _Type = Type>
		auto string(unsigned char delim = ' ') const -> std::enable_if_t<is_streamable<Type, std::ostream>::value, _Type> {
			if(m_items.size() > 0) {
				std::ostringstream ss;
				for(const auto& item : m_items) {
					ss << item << delim;
				}
				auto str = std::move(ss.str());
				str.erase(str.end() - 1); // Removes the last delimiter from the string
				return str;
			}
			return "";
		}

		/// Operators ///

		list_base<Type>& operator=(const list_base<Type>& list) {
			if(this != &list) {
				m_items = list.m_items;
			}
			return *this;
		}

		list_base<Type>& operator=(list_base<Type>&& list) {
			if(this != &list) {
				m_items = std::move(list.m_items);
			}
			return *this;
		}

		void operator+=(const Type& item) { add(item); }

		void operator+=(const list_base<Type>& list) { m_items.insert(m_items.end(), list.m_items.begin(), list.m_items.end()); }

		list_base<Type> operator+(const list_base<Type>& other) {
			list_base<Type> copy;
			copy.get() = m_items;
			copy.get().insert(copy.get().end(), other.get().begin(), other.get().end());
			return copy;
		}

		/// Iterator Functions ///

		auto begin() { return m_items.begin(); }
		auto begin() const { return m_items.begin(); }
		auto end() { return m_items.end(); }
		auto end() const { return m_items.end(); }

		/// Stream Functions ///
		friend std::ostream& operator<<(std::ostream& stream, const list_base<Type>& list) {
			for(const auto& item : list.get()) {
				stream << item << ' ';
			}
			return stream;
		}

	private:
		std::vector<Type> m_items;
	};

	template <typename Type>
	struct ListElement {
		Type value;
		access access_level{ access::self };

		ListElement(const Type& _value) : value(_value) {}
		ListElement(const Type& _value, access _access_level) : value(_value), access_level(_access_level) {}

		template <typename Type2, typename = std::enable_if_t<std::is_convertible_v<Type2, Type>>>
		ListElement(const Type2& _value) : value(_value) {}

		template <typename Type2, typename = std::enable_if_t<std::is_convertible_v<Type2, Type>>>
		ListElement(const Type2& _value, access _access_level) : value(_value), access_level(_access_level) {}

		bool operator<(const ListElement<Type>& other) const { return value < other.value; }

		bool operator==(const ListElement<Type>& other) const { return (value == other.value && access_level == other.access_level); }

		friend std::ostream& operator<<(std::ostream& stream, const ListElement& element) {
			stream << element.value << ' ';
			return stream;
		}
	};

	// TODO: Change this to path_list
	using string_list = list_base<ListElement<std::string>>;
	using path_list = list_base<ListElement<fs::path>>;


	std::optional<fs::path> find_file(const path_list& include_directories, const fs::path& parent_dir, const fs::path& file) {
		if(fs::exists(file)) {
			return file;
		}
		if(fs::exists(parent_dir / file)) {
			return parent_dir / file;
		}

		for(const auto& dir : include_directories) {
			const auto prepended_file_loc = dir.value / file;
			if(fs::exists(prepended_file_loc)) {
				return prepended_file_loc;
			}
		}

		return std::nullopt;
	};

	path_list get_includes_unsorted(const path_list& include_directories, const fs::path& file_name) {
		const fs::path parent_dir = file_name.parent_path();

		path_list includes;

		const auto is_standard_library_header = [&](const std::string& file) -> bool {
			return (std::find(std_header_files.begin(), std_header_files.end(), file) != std_header_files.end());
		};

		std::ifstream file(file_name);
		if(file.is_open()) {

			std::string line;
			while(std::getline(file, line)) {

				if(line.substr(0, 8) == "#include") {
					line = line.substr(8 + (line.at(8) == ' ' ? 1 : 0)); // remove "#include", and if it exists, remove whitespace after it
					line = line.substr(1, line.size() - 2);              // remove the <> or ""

					// check if the include file is part of the standard library or not
					if(!is_standard_library_header(line)) {
						if(auto file_path = find_file(include_directories, parent_dir, fs::path(line))) {

							const fs::path clean_path = file_path.value().lexically_normal();
							includes.get().push_back(clean_path);

							// Check if the dependency has dependencies.
							for(const auto& sub_include_path : get_includes_unsorted(include_directories, clean_path)) {
								includes.get().push_back(sub_include_path);
							}
						}
					}
				}
			}

			file.close();
		}

		return includes;
	}

	path_list get_includes(const path_list& include_directories, const fs::path& file_name) {
		// get list of includes for each cpp file that are unsorted and contains duplicates
		path_list unsorted_includes = get_includes_unsorted(include_directories, file_name);
		auto& data = unsorted_includes.get();

		// Sort and remove duplicates
		std::sort(data.begin(), data.end());
		data.erase(std::unique(data.begin(), data.end()), data.end());
		return unsorted_includes;
	}

} // namespace stdbuild