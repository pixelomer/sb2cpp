#ifndef _SMALLBASIC_TYPES_H
#define _SMALLBASIC_TYPES_H

#include <string>
#include <map>
#include <iostream>
#include <variant>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>

namespace SmallBasic {
	[[ noreturn ]] void Die(std::string message) {
		std::cerr << "*** SmallBasic runtime error: " << message << std::endl;
		throw std::runtime_error(message);
	}

	class Mixed {
	public:
		typedef long double Number;
		typedef std::wstring String;
		typedef std::map<Mixed, Mixed> Array;
	private:
		typedef std::variant<std::nullptr_t, Number, String, Array> _Mixed;
		_Mixed _variant;
		template<typename C>
		bool Compare(const Mixed &b, C cmp) const {
			if (IsString() && b.IsString()) {
				return cmp(GetString(), b.GetString());
			}
			else if (IsNumber() && b.IsNumber()) {
				return cmp(GetNumber(), b.GetNumber());
			}
			else if (IsArray() || b.IsArray()) {
				Die("Cannot compare arrays");
			}
			else {
				//FIXME: Required for std::map, but also allows comparison between
				// other types and undefined values
				return cmp(this->_variant.index(), b._variant.index());
			}
		}
	public:
		Mixed() { }
		Mixed(Number val) { _variant = val; }
		Mixed(String const& val) { _variant = val; }
		Mixed(std::string const& val) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			String str = converter.from_bytes(val);
			_variant = str;
		}
		Mixed(Mixed const& val) { _variant = val._variant; }
		Mixed &operator=(const Mixed &other) {
			_variant = other._variant;
			return *this;
		}
		Mixed operator+(const Mixed &b) const {
			if (IsNumber() && b.IsNumber()) {
				return Mixed(GetNumber() + b.GetNumber());
			}
			else if (IsString() && b.IsString()) {
				return Mixed(GetString() + b.GetString());
			}
			Die("Cannot add incompatible types");
		}
		Mixed operator-(const Mixed &b) const {
			return Mixed(GetNumber() - b.GetNumber());
		}
		Mixed operator/(const Mixed &b) const {
			return Mixed(GetNumber() / b.GetNumber());
		}
		Mixed operator*(const Mixed &b) const {
			return Mixed(GetNumber() * b.GetNumber());
		}
		bool operator<(const Mixed &b) const {
			return Compare(b, std::less{});
		}
		bool operator>(const Mixed &b) const {
			return Compare(b, std::greater{});
		}
		bool operator==(const Mixed &b) const {
			return !(*this > b) && !(*this < b);
		}
		Mixed &operator[](const Mixed &i) {
			return GetArray()[i];
		}
		operator bool() const {
			if (IsNumber()) return GetNumber() != 0;
			else Die("Cannot implicitly convert value to boolean");
		}
		operator Number() const { return GetNumber(); }
		operator String() const { return GetString(); }
		operator Array() { return GetArray(); }
		bool operator>=(const Mixed &b) const { return !(*this < b); }
		bool operator<=(const Mixed &b) const { return !(*this > b); }
		bool operator!=(const Mixed &b) const { return !(*this == b); }
		bool IsUndefined() const { return _variant.index() == 0; }
		bool IsNumber() const { return _variant.index() == 1; }
		bool IsString() const { return _variant.index() == 2; }
		bool IsArray() const { return _variant.index() == 3; }
		Number GetNumber() const {
			if (!IsNumber())
				Die("Expected a Number");
			return std::get<Number>(_variant);
		}
		String Describe() const {
			std::wstringstream stream;
			if (IsString())
				stream << GetString();
			else if (IsNumber())
				stream << GetNumber();
			else if (IsArray())
				stream << L"<array>";
			else
				stream << L"(null)";
			return stream.str();
		}
		const String &GetString() const {
			if (!IsString())
				Die("Expected a String");
			return std::get<String>(_variant);
		}
		String &GetString() {
			if (!IsString())
				Die("Expected a String");
			return std::get<String>(_variant);
		}
		Array &GetArray() {
			if (IsUndefined())
				_variant = Array{{}};
			else if (!IsArray())
				Die("Expected an Array");
			return std::get<Array>(_variant);
		}
	};

	typedef Mixed::Number Number;
	typedef Mixed::String String;
	typedef Mixed::Array Array;
}

#endif