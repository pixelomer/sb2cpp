#ifndef _SMALLBASIC_ARRAY_H
#define _SMALLBASIC_ARRAY_H

#include "Mixed.hpp"
#include <cmath>

namespace SmallBasic {
	class Array {
	public:
		static Mixed ContainsIndex(Mixed &array, Mixed const& index) {
			return array[index].ElementExists();
		}
		static Mixed GetAllIndices(Mixed &array) {
			Mixed result;
			Number index = 1;
			for (auto pair : array.GetArray()) {
				if (pair.first.size() == 1) {
					result[index++] = pair.second;
				}
			}
			return result;
		}
		static Mixed GetItemCount(Mixed &array) {
			Mixed indices = GetAllIndices(array);
			return indices.GetArray().size();
		}
		static Mixed IsArray(Mixed &mixed) {
			return mixed.IsArray();
		}
		static void SetValue(Mixed &array, Mixed const& index, Mixed const& value) {
			array[index] = value;
		}
		static Mixed GetValue(Mixed &array, Mixed const& value) {
			return array[value];
		}
		static void RemoveValue(Mixed &array, Mixed const& index) {
			array[index].RemoveElement();
		}
	};
}

#endif