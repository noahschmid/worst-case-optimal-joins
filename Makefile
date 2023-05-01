all:
	g++ -g -std=c++17 *.cpp -o main

run:
	g++ -std=c++17 *.cpp -o main
	./main

bm_alg_2:
	g++ -std=c++17 tuple_list.cpp table.cpp query.cpp hash_trie.cpp benchmark/algo_2_benchmark.cpp -o bm2.exe -O2

bm_alg_3:
	g++ -std=c++17 tuple_list.cpp table.cpp query.cpp hash_trie.cpp benchmark/algo_3_benchmark.cpp -o bm3.exe -O2
