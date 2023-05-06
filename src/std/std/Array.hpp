#ifndef _SMALLBASIC_ARRAY_H
#define _SMALLBASIC_ARRAY_H

#include "../common/Mixed.hpp"
#include <cmath>

namespace SmallBasic {
	namespace Std {
		class Array {
		private:
			static std::map<String, Mixed> _arrays;
		public:
			static Mixed ContainsIndex(String const& name, Mixed const& index) {
				return Mixed::Boolean(_arrays[name].HasElement(index));
			}
			static Mixed GetAllIndices(Mixed &array) {
				return array.GetArrayIndices();
			}
			static Mixed GetItemCount(Mixed &array) {
				return array.ArrayLength();
			}
			static Mixed IsArray(Mixed &mixed) {
				return Mixed::Boolean(mixed.IsArray());
			}
			static void SetValue(String const& name, Mixed const& index, Mixed const& value) {
				_arrays[name][index] = value;
			}
			static Mixed GetValue(String const& name, Mixed const& value) {
				return _arrays[name][value];
			}
			static void RemoveValue(String const& name, Mixed const& index) {
				_arrays[name].RemoveElement(index);
			}
		};

		std::map<String, Mixed> Array::_arrays = {};
	}
}

#endif