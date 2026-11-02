





#include "Data.hpp"

Data::Data(Parameters *_params) {
    this->params = _params;
    this->dispatch_cost = _params->dispatch_cost;
    this->unit_cost = _params->unit_cost;
}

void Data::read() {
    std::string data_dir = params->data_dir;
    std::string output_dir = params->output_dir;
    std::string dataset = params->dataset;
    std::string filename = params->filename;
    std::string timestamp = params->timestamp;
    try {
        std::ifstream file;
        std::string line;
        std::string placeholder;
        int line_id = 0;
        int id = 0;
        if (dataset.front() == 'W') {
            file.open(data_dir + filename + ".txt");
            while (std::getline(file, line)) {
                if (line_id == 4) {
                    std::istringstream iss(line);
                    iss >> nb_customers >> max_vehicles >> capacity;
                }
                if (line_id >= 9 && line_id <= 9 + nb_customers) {
                    std::istringstream iss(line);
                    double x, y, delivery, pickup, open_time, close_time, service_time;
                    iss >> placeholder >> x >> y >> delivery >> pickup >> open_time >> close_time >> service_time;
                    DataNode node(id, x, y, delivery, pickup, open_time, close_time, service_time);
                    nodes.emplace_back(node);
                    id++;
                }
                ++line_id;
            }
            depot = nodes[0];
        } else if (dataset.front() == 'J') {
            file.open(data_dir + filename + ".vrpsdptw");
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string tag;
                double value;
                iss >> tag;
                if (tag == "DIMENSION") {
                    iss >> placeholder >> value;
                    nb_customers = value - 1;
                } else if (tag == "VEHICLES") {
                    iss >> placeholder >> value;
                    max_vehicles = value;
                } else if (tag == "DISPATCHINGCOST") {
                    iss >> placeholder >> value;
                    dispatch_cost = value;
                } else if (tag == "UNITCOST") {
                    iss >> placeholder >> value;
                    unit_cost = value;
                } else if (tag == "CAPACITY") {
                    iss >> placeholder >> value;
                    capacity = value;
                }

                if (tag == "NODE_SECTION") {
                    int count = 0;
                    while (count < nb_customers + 1) {
                        std::getline(file, line);
                        std::istringstream iss(line);
                        std::string token;
                        std::vector<std::string> tokens;
                        while (std::getline(iss, token, ',')) {
                            tokens.push_back(token);
                        }
                        double id, delivery, pickup, open_time, close_time, service_time;
                        id = std::stod(tokens[0]);
                        delivery = std::stod(tokens[1]);
                        pickup = std::stod(tokens[2]);
                        open_time = std::stod(tokens[3]);
                        close_time = std::stod(tokens[4]);
                        service_time = std::stod(tokens[5]);
                        DataNode node(id, 0, 0, delivery, pickup, open_time, close_time, service_time);
                        nodes.emplace_back(node);
                        count++;
                    }
                }
                if (tag == "DISTANCETIME_SECTION") {
                    distances.resize(nb_customers + 1, std::vector<double>(nb_customers + 1, 0));
                    time_distances.resize(nb_customers + 1, std::vector<double>(nb_customers + 1, 0));
                    for (int i = 0; i < nb_customers + 1; i++) {
                        for (int j = 0; j < nb_customers + 1; j++) {
                            if (i == j) continue;
                            std::getline(file, line);
                            std::istringstream iss(line);
                            std::string token;
                            std::vector<std::string> tokens;
                            while (std::getline(iss, token, ',')) {
                                tokens.push_back(token);
                            }
                            distances[i][j] = std::stod(tokens[2]) * unit_cost;
                            time_distances[i][j] = std::stod(tokens[3]);
                            sum_distance += distances[i][j];
                            sum_time_distance += time_distances[i][j];
                            max_distance = std::max(max_distance, distances[i][j]);
                            min_distance = std::min(min_distance, distances[i][j]);
                        }
                    }
                }
            }
            depot = nodes[0];
            for (int i = 1; i < nb_customers + 1; i++) {
                nodes[i].open_time = std::max(nodes[i].open_time, nodes[0].open_time + time_distances[0][i]);
            }
        } else {
            std::cerr << "[ERROR] dataset not supported" << std::endl;
            exit(1);
        }
        file.close();
    } catch (std::exception &e) {
        std::cerr << "[ERROR] read instance error" << std::endl;
    }
    preprocessing();
}

void Data::preprocessing() {
    calculateDistance();
    calculateFeasibility();


    for (int i = 1; i < nb_customers + 1; i++) {
        delivery_sum += nodes[i].delivery;
        pickup_sum += nodes[i].pickup;
    }

    dist_time_ratio = sum_distance / sum_time_distance;
    std_dist_delta = 2 * (max_distance - min_distance);
    printf("%d customers, max vehicles %d, capacity %.2f, max distance %.2f, min distance %.2f, sum distance %.2f, sum time distance %.2f\n",
           nb_customers, max_vehicles, capacity, max_distance, min_distance, sum_distance, sum_time_distance);

    min_vehicles = std::max(std::ceil((delivery_sum) / capacity), std::ceil((pickup_sum) / capacity));
}

void Data::calculateDistance() {

    if (distances.empty()) {
        distances.resize(nb_customers + 1, std::vector<double>(nb_customers + 1, 0));
        time_distances.resize(nb_customers + 1, std::vector<double>(nb_customers + 1, 0));
        for (int i = 0; i < nb_customers + 1; i++) {
            for (int j = i + 1; j < nb_customers + 1; j++) {
                double dist = sqrt(pow(nodes[i].x - nodes[j].x, 2) + pow(nodes[i].y - nodes[j].y, 2));
                distances[i][j] = dist;
                distances[j][i] = dist;
                time_distances[i][j] = dist;
                time_distances[j][i] = dist;
                sum_distance += dist;
                sum_time_distance += dist;
                if (dist > max_distance) {
                    max_distance = dist;
                }
                if (dist < min_distance) {
                    min_distance = dist;
                }
            }
        }
        for (int i = 1; i < nb_customers + 1; i++) {
            nodes[i].open_time = std::max(nodes[i].open_time, nodes[0].open_time + time_distances[0][i]);
        }
    }
}

void Data::calculateFeasibility() {

    this->feasibility_matrix = std::vector<std::vector<bool>>(nodes.size(), std::vector<bool>(nodes.size(), true));
    int tw_count = 0, cap_count = 0, total_count = 0;
    for (int i = 0; i < nb_customers + 1; ++i) {
        for (int j = 0; j < nb_customers + 1; ++j) {
            if (i != j) {
                bool reduced_by_tw = false;
                bool reduced_by_capacity = false;
                if (nodes[i].open_time + nodes[i].service_time + time_distances[i][j] > nodes[j].close_time) {
                    reduced_by_tw = true;
                }
                if (std::max(nodes[j].open_time, nodes[i].open_time + nodes[i].service_time + time_distances[i][j]) + nodes[j].service_time + time_distances[j][0] > nodes[0].close_time) {
                    reduced_by_tw = true;
                }
                if (((nodes[i].delivery + nodes[j].delivery) > capacity) ||
                    ((nodes[i].pickup + nodes[j].delivery) > capacity) ||
                    ((nodes[i].pickup + nodes[j].pickup) > capacity)) {
                    reduced_by_capacity = true;
                }
                if (reduced_by_tw || reduced_by_capacity) {
                    this->feasibility_matrix[i][j] = false;
                    total_count++;
                }
                if (reduced_by_tw) {
                    tw_count++;
                }
                if (reduced_by_capacity) {
                    cap_count++;
                }
            }
        }
    }
    printf("reduced paths by time window: %d, reduced paths by capacity: %d, total reduced paths: %d\n",
           tw_count, cap_count, total_count);
}
