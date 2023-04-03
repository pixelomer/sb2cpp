#!/usr/bin/env bash

input="$1"
output="$2"

if [ -z "${input}" ]; then
	echo "Usage: $0 <program.sb> [a.out]"
	exit 1
elif [ -z "${output}" ]; then
	output="a.out"
fi

set -ex

./sb2cpp "${input}" > "${input}.cpp"
c++ -O3 --std=c++17 -lm `pkg-config sdl2 --cflags --libs` "${input}.cpp" \
	-o "${output}"