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
c++ --std=c++17 -lm -ObjC++ -framework AppKit -flto -mmacosx-version-min=10.15 \
	-g -fobjc-arc "${input}.cpp" -Wall -Wno-deprecated-declarations \
	-fsanitize=address -o "${output}"