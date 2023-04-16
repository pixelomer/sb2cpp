GIT_COMMIT := $(shell git log -1 --pretty=format:'"\"%H\""')

sb2cpp: src/sb2cpp/main.cpp
	c++ --std=c++14 src/sb2cpp/main.cpp -DGIT_COMMIT=$(GIT_COMMIT) -O3 -o sb2cpp