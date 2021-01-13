# Cardsim
## A quick and dirty card probability simulator.

This is pretty much it. Deals a bunch of hands of cards and counts how many fit some success function.

Compile with `g++ -fopenmp -std=c++17 cards.cpp -O3 -o cards` if you want some nice parallel speedup. Compile with `g++ -DNO_OMP -std=c++17 cards.cpp -O3 -o cards` if you want it single-threaded. Run with `./cards`.
