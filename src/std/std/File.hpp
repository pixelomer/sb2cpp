#ifndef _SMALLBASIC_FILE_H
#define _SMALLBASIC_FILE_H

#include "../common/Mixed.hpp"
#include "../common/StringUtils.hpp"
#include <cmath>
#include <fstream>

namespace SmallBasic {
	namespace Std {
		class File {
		public:
			static String ReadContents(String const& filePath) {
				std::wifstream file;
				file.open(WStringToString(filePath));
				std::wstringstream str_stream;
				str_stream << file.rdbuf();
				String result = str_stream.str();
				file.close();
				return result;
			}
		};
	}
}

#endif