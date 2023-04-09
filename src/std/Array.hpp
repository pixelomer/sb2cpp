#ifndef _SMALLBASIC_ARRAY_H
#define _SMALLBASIC_ARRAY_H

#include "Mixed.hpp"
#include <cmath>

namespace SmallBasic {
	class Array {
	public:
		static Mixed ContainsIndex(Mixed &array, Mixed const& index) {
			return Mixed::Boolean(array.HasElement(index));
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
		static void SetValue(Mixed &array, Mixed const& index, Mixed const& value) {
			array[index] = value;
		}
		static Mixed GetValue(Mixed &array, Mixed const& value) {
			return array[value];
		}
		static void RemoveValue(Mixed &array, Mixed const& index) {
			array.RemoveElement(index);
		}
	};
}

#endif