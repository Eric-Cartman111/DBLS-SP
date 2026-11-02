





#ifndef MA_FIRD_DATA_HPP
#define MA_FIRD_DATA_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "Config.hpp"
#include "Parameters.hpp"
#include "Random.hpp"
#include "Node.hpp"

class Data {
private:
    Parameters *params;
    void preprocessing();
    void calculateDistance();
    void calculateFeasibility();
public:
    int nb_customers = 0;
    int max_vehicles = 0;
    int min_vehicles = 0;
    double capacity = 0;
    double max_distance = 0;
    double min_distance = INF;
    double sum_distance = 0;
    double sum_time_distance = 0;
    double delivery_sum = 0;
    double pickup_sum = 0;
    double dispatch_cost = 0;
    double unit_cost = 0;
    double std_dist_delta = 0;
    double dist_time_ratio = 0;
    DataNode depot;
    std::vector<DataNode> nodes;
    std::vector<std::vector<double>> distances;
    std::vector<std::vector<double>> time_distances;
    std::vector<std::vector<bool>> feasibility_matrix;
    Data(Parameters *_params);
    Data() = default;
    ~Data() = default;
    void read();
};

#endif
