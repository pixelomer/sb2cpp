#ifndef _SMALLBASIC_H
#define _SMALLBASIC_H

#include "common/Mixed.hpp"
#include "std/Math.hpp"
#include "std/Stack.hpp"
#include "std/Text.hpp"
#include "std/TextWindow.hpp"
#include "std/Program.hpp"
#include "std/Array.hpp"
#include "std/GraphicsWindow.hpp"
#include "std/Timer.hpp"
#include "std/File.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

class ForIterator : public std::iterator<std::output_iterator_tag, SmallBasic::Mixed> {
private:
	SmallBasic::Mixed *_indexPt;
	SmallBasic::Mixed _index;
	SmallBasic::Mixed _step;
	SmallBasic::Mixed _end;
public:
	ForIterator(SmallBasic::Mixed *indexPt, SmallBasic::Mixed step,
		SmallBasic::Mixed end): _indexPt(indexPt), _index(*indexPt), _step(step),
		_end(end) {}
	reference operator*() const { return *_indexPt; }
	pointer operator->() { return _indexPt; }
	bool finished() const {
		if (_step <= SmallBasic::Mixed(0.L) && _index < _end) {
			return true;
		}
		if (_step >= SmallBasic::Mixed(0.L) && _index > _end) {
			return true;
		}
		return false;
	}
	ForIterator &operator++() {
		if (!finished()) {
			_index = *_indexPt = *_indexPt + _step;
		}
		return *this;
	}
	ForIterator operator++(int) {
		ForIterator iterator = *this;
		++(*this);
		return iterator;
	}
	friend bool operator==(const ForIterator& a, const ForIterator& b) {
		return a.finished() && b.finished();
	}
	friend bool operator!=(const ForIterator& a, const ForIterator& b) {
		return !(a == b);
	}
	ForIterator begin() { return *this; }
  ForIterator end() {
		ForIterator iterator = ForIterator(_indexPt, _step, _end);
		iterator._index = _end + _step;
		return iterator;
	}
};

#pragma GCC diagnostic pop

#if __cplusplus >= 201703
[[maybe_unused]]
#else
__attribute__((unused))
#endif
static ForIterator ForLoop(SmallBasic::Mixed *index, SmallBasic::Mixed step,
	SmallBasic::Mixed end)
{
	return ForIterator(index, step, end);
}

#endif