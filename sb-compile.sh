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

c++ -I/opt/local/include `sdl2-config --cflags` `sdl2-config --static-libs` \
	--std=c++14 "${input}.cpp" -Wall -flto -O3 -Wno-reorder-ctor -o "${output}"