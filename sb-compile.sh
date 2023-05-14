#!/usr/bin/env bash

input="$1"
output="$2"

if [ -z "${input}" ]; then
	echo "Usage: $0 <program.sb> [a.out]"
	exit 1
elif [ -z "${output}" ]; then
	output="a.out"
fi

./sb2cpp "${input}" > "${input}.cpp"

c++ -Isrc -I/opt/local/include -L/opt/local/lib `sdl2-config --cflags --libs` \
	-lSDL2_ttf -O3 -flto --std=c++14 -include Liberation.hpp "${input}.cpp" -Wall \
	-Wno-reorder-ctor -o "${output}"