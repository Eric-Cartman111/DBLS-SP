





#ifndef MA_FIRD_ROUTE_HPP
#define MA_FIRD_ROUTE_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Random.hpp"
#include "Parameters.hpp"
#include "Data.hpp"

class Route {
private:
    static Random *random;
    static Parameters *params;
    static Data *data;
public:
    Node *head = nullptr;
    int id = 0;
    int size = 0;
    bool removed = false;
    bool tw_feasible = true;
    bool cap_feasible = true;
    double cost = 0;
    double reverse_time = 0;
    double max_load = 0;
    double overload = 0;
    std::vector<Node*> nodes;
    std::vector<std::vector<double>> cost_matrix, in_load_matrix, out_load_matrix, max_load_matrix,
        open_time_matrix, close_time_matrix, duration_time_matrix, wait_time_matrix, reverse_time_matrix;
    Route(DataNode &node);
    Route(const Route& route);
    Route() = default;
    ~Route();
    static void setContext(Random *_random, Parameters *params, Data *data);
    void update();
    void updateMatrix();
    void print() const;
    void insert(Node* node);
    void insert(Node* pos, Node* node);
    void insert(Node* prev, Node *next, SuperNode &node);
    void remove(Node *pos);
    void remove(SuperNode &node);
};

#endif
