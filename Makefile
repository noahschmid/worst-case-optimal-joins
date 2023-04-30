all:
	g++ -std=c++17 *.cpp -o main

run:
	g++ -std=c++17 *.cpp -o main
	./main

bm_alg_2:
	g++ -std=c++17 tuple_list.cpp table.cpp query.cpp hash_trie.cpp benchmark/algo_2_benchmark.cpp -o bm.exe
