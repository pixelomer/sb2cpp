#ifndef _SMALLBASIC_TYPES_H
#define _SMALLBASIC_TYPES_H

#include <string>
#include <map>
#include <iostream>
#include <variant>
#include <sstream>
#include <locale>
#include <codecvt>
#include <vector>
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
		typedef std::map<std::vector<Mixed>, Mixed> Array;
	private:
		typedef std::variant<std::nullptr_t, Number, String, Array> _Mixed;
		std::shared_ptr<_Mixed> _variant;
		std::vector<Mixed> _index;
		static Array _nullArray;
		template<typename C>
		bool Compare(const Mixed &b, C cmp) const {
			if (IsNumber() || b.IsNumber())
				return cmp(GetNumber(), b.GetNumber());
			else
				return cmp(GetString(), b.GetString());
		}
		bool _IsArray() const { return _variant->index() == 3; }
		Array &_GetArray() {
			if (!_IsArray()) {
				*_variant = Array();
			}
			return std::get<Array>(*_variant);
		}
		Array &_GetArray() const {
			if (!_IsArray()) {
				_nullArray.clear();
				return _nullArray;
			}
			return std::get<Array>(*_variant);
		}
		_Mixed &operator*() {
			if (_index.size() != 0) {
				return *((_GetArray())[_index]._variant);
			}
			else {
				return *_variant;
			}
		}
		const _Mixed &operator*() const {
			if (_index.size() != 0) {
				return *((_GetArray())[_index]._variant);
			}
			else {
				return *_variant;
			}
		}
	public:
		Mixed() { _variant = std::make_shared<_Mixed>(nullptr); }
		Mixed(Number val) { _variant = std::make_shared<_Mixed>(val); }
		Mixed(String const& val) { _variant = std::make_shared<_Mixed>(val); }
		Mixed(std::string const& val) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			String str = converter.from_bytes(val);
			_variant = std::make_shared<_Mixed>(str);
		}
		Mixed(Mixed const& val, bool copy = true) {
			if (copy) {
				_variant = std::make_shared<_Mixed>(nullptr);
				*this = val;
			}
			else {
				_variant = val._variant;
				_index = val._index;
			}
		}
		Mixed operator[](Mixed const& key) {
			Mixed sub(*this, false);
			sub._index.push_back(key);
			return sub;
		}
		Mixed &operator=(Mixed const& val) {
			**this = *val._variant;
			return *this;
		}
		Mixed operator+(const Mixed &b) const {
			return Mixed(GetNumber() + b.GetNumber());
		}
		Mixed operator-(const Mixed &b) const {
			return Mixed(GetNumber() - b.GetNumber());
		}
		Mixed operator-() const {
			return Mixed(-GetNumber());
		}
		Mixed operator+() const {
			return Mixed(GetNumber());
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
		operator bool() const { return GetNumber() != 0; }
		operator Number() const { return GetNumber(); }
		operator String() const { return GetString(); }
		bool operator>=(const Mixed &b) const { return !(*this < b); }
		bool operator<=(const Mixed &b) const { return !(*this > b); }
		bool operator!=(const Mixed &b) const { return !(*this == b); }
		bool IsUndefined() const { return (**this).index() == 0; }
		bool IsNumber() const { return (**this).index() == 1; }
		bool IsString() const { return (**this).index() == 2; }
		bool IsArray() const { return (**this).index() == 3; }
		bool IsArray() { return (**this).index() == 3; }
		Number GetNumber() const {
			if (IsNumber())
				return std::get<Number>(**this);
			else if (IsString())
				return std::stold(GetString());
			return 0.L;
		}
		String GetString() const {
			if (IsString()) {
				return std::get<String>(**this);
			}
			std::wstringstream stream;
			if (IsArray()) {
				stream << "<array>";
			}
			else if (IsNumber())
				stream << GetNumber();
			return stream.str();
		}
		Array &GetArray() {
			if (!IsArray()) {
				**this = Array();
			}
			return std::get<Array>(**this);
		}
	};

	typedef Mixed::Number Number;
	typedef Mixed::String String;
	typedef Mixed::Array Array;

	Array Mixed::_nullArray = Array();
}

#endif