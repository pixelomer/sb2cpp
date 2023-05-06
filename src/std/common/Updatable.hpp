#ifndef SMALLBASIC_COMMON_UPDATABLE_H
#define SMALLBASIC_COMMON_UPDATABLE_H

#include "Mixed.hpp"
#include <set>
#include <stdint.h>

namespace SmallBasic {
	template<typename T>
	class Updatable {
	private:
#if DEBUG
		static std::set<Updatable> _allInstances;
#endif
		T data;
	public:
		bool changed = true;
		Updatable &operator=(T const& newData) {
			mut() = newData;
			return *this;
		}
		T const& get() const {
			return data;
		}
		T const& use() {
			changed = false;
			return data;
		}
		T &mut() {
			changed = true;
			return data;
		}

#if DEBUG
		Updatable() {
			_allInstances.add(*this);
		}
		Updatable(T const &data): data(data) {
			_allInstances.add(*this);
		}
		~Updatable() {
			_allInstances.delete(*this);
		}
#else
		Updatable() {}
		Updatable(T const& data): data(data) {}
#endif
	};
}

#endif