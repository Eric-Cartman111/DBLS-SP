





#include "Generator.hpp"


Random *Generator::random = nullptr;
Parameters *Generator::params = nullptr;
Data *Generator::data = nullptr;

void Generator::setContext(Random *_random, Parameters *_params, Data *_data) {
    Generator::random = _random;
    Generator::params = _params;
    Generator::data = _data;
    RLAgent::setContext(_random, _params, _data);
}

void Generator::init() {
    if (params->population_size == 1) return;
    max_parents_num = std::min(params->population_size, 10);
    n_parents = random->randomInt(max_parents_num - 1) + 2;
    ratio = params->init_ratio;

    parent_agent.init(max_parents_num - 1, 1);

    auto num_insertion = 5;
    insert_opt = random->randomInt(num_insertion);
    insert_agent.init(num_insertion, 1);
    if (params->mab_ratio) {
        ratio_opt = random->randomInt(ratio_values.size());
        ratio = ratio_values[ratio_opt];
        ratio_agent.init(ratio_values.size(), 1);
    }
}

void Generator::update(std::vector<Solution *> &parents, Solution *offspring) {
    double reward;
    if (!offspring->feasible) {
        reward = -2 * abs_sum_reward / step;
    } else {
        reward = parents[0]->total_cost - offspring->total_cost;
    }
    abs_sum_reward += abs(reward);
    step++;

    if (!params->mab_ratio && !offspring->feasible) {
        ratio = std::min(ratio * 1.02, 0.999);
    }

    parent_agent.updateQValues(0, n_parents - 2, 0, reward);
    n_parents = parent_agent.selectAction(0) + 2;

    insert_agent.updateQValues(0, insert_opt, 0, reward);
    insert_opt = insert_agent.selectAction(0);
    if (params->mab_ratio) {
        ratio_agent.updateQValues(0, ratio_opt, 0, reward);
        ratio_opt = ratio_agent.selectAction(0);
        ratio = ratio_values[ratio_opt];
    }
}

std::vector<int> Generator::selectParents(int pop_size) {
    std::vector<int> indices;
    if (pop_size < 2) {
        std::cerr << "[ERROR] no enough individuals in population" << std::endl;
        exit(1);
    }
    indices = random->randomInts(pop_size);

    if (pop_size > n_parents) {
        indices.erase(indices.begin() + n_parents, indices.end());
    } else {
        n_parents = pop_size;
    }
    return indices;
}

Solution *Generator::breed(std::vector<Solution *> &parents) {
    Solution* offspring = ARIX(parents);
    return offspring;
}

Solution *Generator::ARIX(const std::vector<Solution *>& parents) {
    auto dominant = parents[0];
    auto solution = new Solution();
    auto route_size = dominant->routes.size();
    auto route_cost_per_customer = [](Route *route) {
        return route->cost / (route->nodes.size() - 1);
    };
    auto route_less = [&route_cost_per_customer](const std::pair<Route*, double> &a,
                                                 const std::pair<Route*, double> &b) {
        if (a.second != b.second) {
            return a.second < b.second;
        }
        return route_cost_per_customer(a.first) < route_cost_per_customer(b.first);
    };
    std::vector<bool> visited(data->nb_customers + 1, false);

    std::vector<std::pair<Route*, double>> dominant_routes, routes_pool;
    for (auto &route : dominant->routes) {
        dominant_routes.emplace_back(route, 0);
    }

    for (int i = 1; i < n_parents; ++i) {
        for (auto &route : parents[i]->routes) {
            routes_pool.emplace_back(route, 0);
        }
    }
    int retained_route_size = std::max(floor(dominant->routes.size() * ratio), 1.0);
    int introduced_route_size = route_size - retained_route_size;
    if (introduced_route_size > routes_pool.size()) {
        introduced_route_size = routes_pool.size();
        retained_route_size = route_size - introduced_route_size;
    }
    if (retained_route_size--) {

        auto route = new Route(*dominant_routes[random->randomInt(dominant_routes.size())].first);
        solution->routes.emplace_back(route);

        auto it = std::find_if(dominant_routes.begin(), dominant_routes.end(), [route](std::pair<Route*, double> a) {
            return a.first->cost == route->cost;
        });
        dominant_routes.erase(it);

        Node *p = route->head->next;
        while (p != route->head) {
            visited[p->id] = true;
            p = p->next;
        }

        for (auto &rt : routes_pool) {
            rt.second = 0;
            Node *q = rt.first->head->next;
            while (q != rt.first->head) {
                if (visited[q->id]) {
                    rt.second += 1;
                }
                q = q->next;
            }
        }
    }

    while (solution->routes.size() < route_size) {
        if (introduced_route_size-- > 0) {

            for (auto &route : routes_pool) {
                route.second = route.second / (route.first->nodes.size() - 1);
            }

            std::sort(routes_pool.begin(), routes_pool.end(), route_less);
            auto least_repeated = routes_pool[0].second;
            auto least_cost_per_customer = route_cost_per_customer(routes_pool[0].first);

            std::vector<Route*> routes;
            for (auto &route : routes_pool) {
                if (route.second == least_repeated &&
                    route_cost_per_customer(route.first) == least_cost_per_customer) {
                    routes.emplace_back(route.first);
                } else {
                    break;
                }
            }

            auto route = new Route(*routes[random->randomInt(routes.size())]);
            solution->routes.emplace_back(route);

            auto it = std::find_if(routes_pool.begin(), routes_pool.end(), [route](std::pair<Route*, double> a) {
                return a.first->cost == route->cost;
            });
            routes_pool.erase(it);

            Node *p = route->head->next;
            while (p != route->head) {
                visited[p->id] = true;
                p = p->next;
            }

            for (auto &rt : dominant_routes) {
                rt.second = 0;
                Node *q = rt.first->head->next;
                while (q != rt.first->head) {
                    if (visited[q->id]) {
                        rt.second += 1;
                    }
                    q = q->next;
                }
            }
            for (auto &rt : routes_pool) {
                rt.second = 0;
                Node *q = rt.first->head->next;
                while (q != rt.first->head) {
                    if (visited[q->id]) {
                        rt.second += 1;
                    }
                    q = q->next;
                }
            }
        }

        if (retained_route_size-- > 0) {

            for (auto &route : dominant_routes) {
                route.second = route.second / (route.first->nodes.size() - 1);
            }

            std::sort(dominant_routes.begin(), dominant_routes.end(), route_less);

            auto least_repeated = dominant_routes[0].second;
            auto least_cost_per_customer = route_cost_per_customer(dominant_routes[0].first);

            std::vector<Route*> routes;
            for (auto &route : dominant_routes) {
                if (route.second == least_repeated &&
                    route_cost_per_customer(route.first) == least_cost_per_customer) {
                    routes.emplace_back(route.first);
                } else {
                    break;
                }
            }

            auto route = new Route(*routes[random->randomInt(routes.size())]);
            solution->routes.emplace_back(route);

            auto it = std::find_if(dominant_routes.begin(), dominant_routes.end(), [route](std::pair<Route*, double> a) {
                return a.first->cost == route->cost;
            });
            dominant_routes.erase(it);

            Node *p = route->head->next;
            while (p != route->head) {
                visited[p->id] = true;
                p = p->next;
            }

            for (auto &rt : routes_pool) {
                rt.second = 0;
                Node *q = rt.first->head->next;
                while (q != rt.first->head) {
                    if (visited[q->id]) {
                        rt.second += 1;
                    }
                    q = q->next;
                }
            }
        }
    }

    visited.assign(data->nb_customers + 1, false);
    for (auto &route : solution->routes) {
        Node *p = route->head->next;
        while (p != route->head) {
            Node *q = nullptr;
            if (visited[p->id]) {
                route->remove(p);
                q = p;
            } else {
                visited[p->id] = true;
            }
            p = p->next;
            if (q != nullptr) {
                delete q;
            }
        }
        route->update();
    }

    std::vector<Node*> nodes;
    for (int i = 1; i <= data->nb_customers; i++) {
        if (!visited[i]) {
            nodes.emplace_back(new Node(data->nodes[i]));
        }
    }
    random->shuffle(nodes);
    solution->insertion(nodes, insert_opt);


    if (!params->mab_ratio) {
        if (solution->nb_vehicle > dominant->nb_vehicle) {
            ratio = std::min(ratio * 1.01, 0.999);
        } else if (solution->nb_vehicle == dominant->nb_vehicle) {
            ratio = std::max(ratio * 0.99, 0.001);
        }
    }

    return solution;
}
