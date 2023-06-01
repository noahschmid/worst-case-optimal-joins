## how to compile and run
```shell
# compile
make
# run
./main
```

## how to get started
read `main.cpp` as sample usage code.

## how to run measurements
1. Do `make bm_alg_2`, `make bm_alg_3`, or `make e2e` depending on which
   measurement you want to run.
2. In the project root directory, run `python -m benchmark.algo_2_measure`, `python -m benchmark.algo_3_measure`,
or `python -m benchmark.e2e_measure`.  

## how to run tests
```shell
make test
```

## miscellaneous

C++ is chosen as the source file type as we only have to
write C code for the most performance-critical parts, and for
other boilerplate code, we can borrow C++ features to facilitate
development.