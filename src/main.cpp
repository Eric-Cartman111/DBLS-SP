





#include <iostream>
#include "Algorithm.hpp"

int main(int argc, char **argv) {
    std::cout << "DBLS-SP: Dynamic Balanced Local Search with Solution Pool for VRPSPDTW" << std::endl << std::endl;
    auto params = new Parameters(argc, argv);
    params->print();
    Algorithm algo(params);
    algo.run();
    return 0;
}
