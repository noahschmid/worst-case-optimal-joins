SRC_FILES_MAIN = $(filter-out test.cpp, $(wildcard *.cpp))
SRC_FILES_TEST = $(filter-out main.cpp, $(wildcard *.cpp))

all:
	g++ -O3 -g -std=c++17 $(SRC_FILES_MAIN) -o main

run:
	g++ -O3 -std=c++17 $(SRC_FILES_MAIN) -o main
	./main

test:
	g++ -O3 -std=c++17 $(SRC_FILES_TEST) -o test
	./test

bm_alg_2:
	g++ -std=c++17 tuple_list.cpp table.cpp query.cpp hash_trie.cpp benchmark/algo_2_benchmark.cpp -o bm2.exe -O2

bm_alg_3:
	g++ -std=c++17 tuple_list.cpp table.cpp query.cpp hash_trie.cpp benchmark/algo_3_benchmark.cpp -o bm3.exe -O2
