#ifndef SMALLBASIC_COMMON_MIXED_H
#define SMALLBASIC_COMMON_MIXED_H

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
		typedef std::map<Mixed, Mixed> Array;
	private:
		static Number _unique;
		enum MixedType {
			MIXED_UNDEFINED,
			MIXED_NUMBER,
			MIXED_STRING,
			MIXED_ARRAY
		};
		Mixed *_lastIndex = NULL;
		Number _number = 0.L;
		String _string = L"";
		Array _array;
		enum MixedType _type = MIXED_UNDEFINED;
		static Array ParseArray(String const& str) {
			Array array;
			size_t i = 0;
			while (i < str.length()) {
				size_t start = i;
				i = str.find(L'=', start);
				if (i == String::npos) break;
				Mixed index = str.substr(start, i-start);
				i++;
				if (i >= str.length()) break;
				start = i;
				i = str.find(L';', start);
				if (i == String::npos) break;
				Mixed value = str.substr(start, i-start);
				array[index] = value;
				i++;
			}
			return array;
		}

		static Array CopyArray(Array const& old) {
			Array copy;
			for (auto pair : old) {
				if (pair.second.IsArray()) {
					copy[pair.first] = CopyArray(pair.second._array);
				}
				else {
					copy[pair.first] = pair.second;
				}
			}
			return copy;
		}

		Array GetArray() const {
			if (IsArray()) {
				return _array;
			}
			else if (IsString()) {
				return ParseArray(_string);
			}
			return {};
		}

		bool _HasElement(Mixed const& index) {
			return IsArray() && (_array.count(index) > 0);
		}

		void RemoveEmptyValues() {
			if (IsArray() && _lastIndex != NULL && _lastIndex->HasValue()) {
				if (_HasElement(*_lastIndex)) {
					Mixed &value = _array[*_lastIndex];
					if (!value.HasValue()) {
						_array.erase(*_lastIndex);
						_lastIndex->_type = MIXED_UNDEFINED;
					}
				}
			}
		}
	public:
		Mixed() {}
		~Mixed() {
			if (_lastIndex != NULL) {
				delete _lastIndex;
			}
		}
		Mixed(String const& value) { *this = value; }
		Mixed(const wchar_t *value) { *this = String(value); }
		Mixed(Number value) { *this = value; }
		Mixed(Array const& value) { *this = value; }
		Mixed(Mixed const& value) { *this = value; }
		Mixed(std::string const& value) { *this = value; }

		static Mixed MakeUnique() {
			Mixed mixed = Mixed(_unique);
			_unique += 1.L;
			return mixed;
		}

		static Mixed Boolean(bool condition) {
			return Mixed(condition ? L"True" : L"False");
		}

		bool operator<(Mixed const& b) const {
			try { return TryGetNumber() < b.TryGetNumber(); }
			catch (...) { return GetString() < b.GetString(); }
		}
		bool operator>(Mixed const& b) const {
			try { return TryGetNumber() > b.TryGetNumber(); }
			catch (...) { return GetString() > b.GetString(); }
		}
		bool operator==(Mixed const& b) const {
			try { return TryGetNumber() == b.TryGetNumber(); }
			catch (...) { return GetString() == b.GetString(); }
		}
		bool operator!=(Mixed const& b) const {
			try { return TryGetNumber() != b.TryGetNumber(); }
			catch (...) { return GetString() != b.GetString(); }
		}
		bool operator>=(Mixed const& b) const {
			try { return TryGetNumber() >= b.TryGetNumber(); }
			catch (...) { return GetString() >= b.GetString(); }
		}
		bool operator<=(const Mixed &b) const {
			try { return TryGetNumber() <= b.TryGetNumber(); }
			catch (...) { return GetString() <= b.GetString(); }
		}

		bool HasValue() const { return _type != MIXED_UNDEFINED; }
		bool IsArray() const { return _type == MIXED_ARRAY; }
		bool IsNumber() const { return !HasValue() || (_type == MIXED_NUMBER); }
		bool IsString() const { return !HasValue() || (_type == MIXED_STRING); }

		Mixed &operator=(String const& str) {
			_string = str;
			_type = MIXED_STRING;
			return *this;
		}

		Mixed &operator=(std::string const& utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			String str = converter.from_bytes(utf8);
			_string = str;
			_type = MIXED_STRING;
			return *this;
		}

		Mixed &operator=(Array const& array) {
			_array = CopyArray(array);
			_type = MIXED_ARRAY;
			return *this;
		}

		Mixed &operator=(Number num) {
			_number = num;
			_type = MIXED_NUMBER;
			return *this;
		}

		Mixed &operator=(Mixed const& mixed) {
			_type = mixed._type;
			switch (mixed._type) {
				case MIXED_ARRAY: *this = mixed._array; break;
				case MIXED_NUMBER: *this = mixed._number; break;
				case MIXED_STRING: *this = mixed._string; break;
				case MIXED_UNDEFINED: break;
			}
			return *this;
		}

		operator Number() const {
			return GetNumber();
		}

		operator std::string() const {
			String str = GetString();
			char buf[MB_CUR_MAX + 1];
			std::string result;
			std::wctomb(NULL, 0);
			for (wchar_t c : str) {
				int len = std::wctomb(buf, c);
				if (len < 0) {
					buf[0] = '?';
					len = 1;
				}
				buf[len] = '\0';
				result += buf;
			}
			return result;
		}

		operator String() const {
			return GetString();
		}

		operator bool() const {
			return GetBoolean();
		}

		Mixed operator+(Mixed const& b) {
			if (IsNumber() && b.IsNumber()) {
				return GetNumber() + b.GetNumber();
			}
			return GetString() + b.GetString();
		}

		Mixed operator-(Mixed const& b) {
			return GetNumber() - b.GetNumber();
		}

		Mixed operator*(Mixed const& b) {
			return GetNumber() * b.GetNumber();
		}

		Mixed operator/(Mixed const& b) {
			return GetNumber() / b.GetNumber();
		}

		Mixed operator+() {
			return GetNumber();
		}

		Mixed operator-() {
			return -GetNumber();
		}

		Mixed &operator[](Mixed const& index) {
			if (IsNumber()) {
				_array.clear();
			}
			else if (IsString()) {
				_array = ParseArray(GetString());
			}
			_type = MIXED_ARRAY;
			RemoveEmptyValues();
			if (_lastIndex == NULL) {
				_lastIndex = new Mixed();
			}
			*_lastIndex = index;
			return _array[index];
		}

		String GetString() const {
			if (IsString()) {
				return _string;
			}
			std::wstringstream stream;
			if (IsNumber()) {
				stream << _number;
			}
			else if (IsArray()) {
				for (auto pair : _array) {
					stream << pair.first.GetString() << L"=";
					stream << pair.second.GetString() << L";";
				}
			}
			return stream.str();
		}

		Number TryGetNumber() const {
			if (IsNumber()) {
				return _number;
			}
			else {
				return std::stold(GetString());
			}
		}

		Number GetNumber() const {
			try { return TryGetNumber(); }
			catch (...) { return 0.L; }
		}

		bool GetBoolean() const {
			return GetString() == L"True";
		}

		Array GetArrayIndices() {
			Array indices;
			Number index = 1;
			RemoveEmptyValues();
			for (auto pair : GetArray()) {
				indices[index++] = pair.first;
			}
			return indices;
		}

		bool HasElement(Mixed const& index) {
			RemoveEmptyValues();
			return _HasElement(index);
		}

		Number ArrayLength() {
			if (IsArray()) {
				RemoveEmptyValues();
				return _array.size();
			}
			else {
				return 0.L;
			}
		}

		void RemoveElement(Mixed const& index) {
			if (IsArray()) {
				_array.erase(index);
			}
		}
	};

	typedef Mixed::Number Number;
	typedef Mixed::String String;
	Number Mixed::_unique = 1;
}

#endif