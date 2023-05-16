#!/usr/bin/env bash

# The standard library supports C++14 but relies on some platform-specific and
# compiler-specific code. Use C++17 whenever possible to use platform-independent
# equivalents.

input="$1"
output="$2"

if [ -z "${input}" ]; then
	echo "Usage: $0 <program.sb> [a.out]"
	exit 1
elif [ -z "${output}" ]; then
	output="a.out"
fi

./sb2cpp "${input}" > "${input}.cpp"

c++ -Isrc `sdl2-config --cflags --libs` \
	-lSDL2_ttf -O3 -flto --std=c++17 -include Liberation.hpp "${input}.cpp" -Wall \
	-o "${output}"