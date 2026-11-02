





#ifndef MA_FIRD_RANDOM_HPP
#define MA_FIRD_RANDOM_HPP

#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>
#include "Config.hpp"

class Random {
private:
    std::mt19937 generator;
public:
    Random() = default;
    ~Random() = default;
    Random(int seed) {
        srand(seed);
        generator.seed(seed);
    }


    int randomInt(int n) {
        if (n <= 0) throw std::invalid_argument("[ERROR] n must be positive");
        return rand() % n;
    }


    double randomDouble(double a, double b) {
        std::uniform_real_distribution<double> distribution(a, b);
        return distribution(generator);
    }


    template <typename T>
    void shuffle(std::vector<T> &vec) {
        std::shuffle(vec.begin(), vec.end(), generator);
    }


    std::vector<int> randomInts(int n) {
        std::vector<int> ints(n);
        std::iota(ints.begin(), ints.end(), 0);
        shuffle(ints);
        return ints;
    }
};


#endif
