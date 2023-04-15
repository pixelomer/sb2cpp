#ifndef _SMALLBASIC_FILE_H
#define _SMALLBASIC_FILE_H

#include "Mixed.hpp"
#include <cmath>
#include <fstream>

namespace SmallBasic {
	class File {
	public:
		static String ReadContents(std::string const& filePath) {
			std::wifstream file;
			file.open(filePath);
			std::wstringstream str_stream;
			str_stream << file.rdbuf();
			String result = str_stream.str();
			file.close();
			return result;
		}
	};
}

#endif