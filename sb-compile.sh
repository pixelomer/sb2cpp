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
c++ --std=c++14 -lm -ObjC++ -framework AppKit -mmacosx-version-min=10.9 \
	-fobjc-arc "${input}.cpp" -Wall -O0 -g -Wno-reorder-ctor -o "${output}"