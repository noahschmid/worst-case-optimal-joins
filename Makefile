SRC_FILES_MAIN = $(filter-out test.cpp, $(wildcard *.cpp))
SRC_FILES_TEST = $(filter-out main.cpp, $(wildcard *.cpp))
SRC_FILES_BM = $(filter-out main.cpp test.cpp, $(wildcard *.cpp))

.PHONY: all run test

all:
	g++ -O3 -std=c++17 $(SRC_FILES_MAIN) -o main

run:
	g++ -O3 -std=c++17 $(SRC_FILES_MAIN) -o main
	./main

test:
	rm test
	g++ -O3 -std=c++17 $(SRC_FILES_TEST) -o test
	./test

debug:
	g++ -O0 -g -std=c++17 $(SRC_FILES_MAIN) -o main

bm_alg_2:
	g++ -std=c++17 $(SRC_FILES_BM) benchmark/algo_2_benchmark.cpp -o bm2.exe -O2

bm_alg_3:
	g++ -std=c++17 $(SRC_FILES_BM) benchmark/algo_3_benchmark.cpp -o bm3.exe -O2
