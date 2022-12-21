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

	using string_list = list_base<ListElement<std::string>>;
	using path_list = list_base<ListElement<fs::path>>;

} // namespace stdbuild