





#include "Population.hpp"

Random *Population::random = nullptr;
Parameters *Population::params = nullptr;
Data *Population::data = nullptr;

Population::Population() = default;

Population::~Population() {
    for (auto &individual: population) {
        delete individual;
    }
    population.clear();
    for (auto &entry: archive) {
        delete entry.second;
    }
    archive.clear();
    delete best_solution;
}

void Population::setContext(Random *_random, Parameters *_params, Data *_data) {
    Population::random = _random;
    Population::params = _params;
    Population::data = _data;
    Searcher::setContext(_random, _params, _data);
    Generator::setContext(_random, _params, _data);
}

void Population::init() {
    generator.init();
    bool found_initial_solution = false;
    int initial_nb_vehicle = 0;
    double initial_cost = 0, initial_total_cost = INF;
    while (population.size() < params->population_size) {
        auto solution = new Solution();
        solution->init();
        if (!found_initial_solution || solution->total_cost < initial_total_cost - PRECISION) {
            found_initial_solution = true;
            initial_nb_vehicle = solution->nb_vehicle;
            initial_cost = solution->cost;
            initial_total_cost = solution->total_cost;
        }
        solution = searcher.FIRD(solution);
        updateArchive(solution);
        addInitial(solution);
    }
    if (found_initial_solution) {
        std::cout << "RCRS initialization finished. Best solution before FIRD has "
                  << initial_nb_vehicle << " vehicles, distance cost " << initial_cost
                  << " and total cost " << initial_total_cost << std::endl;
    } else {
        std::cout << "RCRS initialization finished without a solution." << std::endl;
    }
}

void Population::next(bool use_archive_guided) {
    if (population.empty() || population.size() == 1) {
        std::cerr << "[ERROR] there is no enough individuals in the population" << std::endl;
        exit(1);
    }
    Solution *solution;
    if (use_archive_guided) {
        solution = archiveRouteDescent();
    } else {
        auto indices = generator.selectParents(population.size());
        std::vector<Solution*> parents;
        for (auto &index: indices) {
            parents.emplace_back(population[index]->solution);
        }
        solution = generator.breed(parents);
        solution = searcher.FIRD(solution);
        generator.update(parents, solution);
    }

    updateArchive(solution);
    if (add(solution) && population.size() >= 1.5 * params->population_size) {
        update();
    }

}

bool Population::add(Solution *solution) {
    if (params->qf_nnr && population.size() >= params->population_size) {
        return addByQualityNearestNeighbor(solution);
    }
    return addByFitnessDistance(solution);
}

bool Population::addInitial(Solution *solution) {
    double min_distance = INF;
    for (auto &individual: population) {
        double distance = solution->distance(individual->solution);
        individual->distance = std::min(individual->distance, distance);
        min_distance = std::min(min_distance, distance);
    }

    if (solution->betterThan(best_solution)) {
        delete best_solution;
        best_solution = new Solution(*solution);
    }

    auto individual = new Individual(solution);
    individual->distance = min_distance;
    population.emplace_back(individual);
    return true;
}

bool Population::addByFitnessDistance(Solution *solution) {
    std::vector<double> distances(population.size());
    double min_distance = INF;

    for (int i = 0; i < population.size(); ++i) {
        double distance = solution->distance(population[i]->solution);
        distances[i] = distance;
        min_distance = std::min(min_distance, distance);
    }
    bool found_better = solution->betterThan(best_solution);
    if (found_better || solution->feasible && min_distance > PRECISION) {
        if (found_better) {
            delete best_solution;
            best_solution = new Solution(*solution);
        }

        for (int i = 0; i < population.size(); ++i) {
            population[i]->distance = std::min(population[i]->distance, distances[i]);
        }

        auto individual = new Individual(solution);
        individual->distance = min_distance;
        population.emplace_back(individual);
        return true;
    } else {
        delete solution;
        return false;
    }
}

bool Population::addByQualityNearestNeighbor(Solution *solution) {
    if (solution == nullptr || !solution->feasible) {
        delete solution;
        return false;
    }

    if (betterForReplacement(solution, best_solution)) {
        int worst_index = findWorstIndividual();
        delete best_solution;
        best_solution = new Solution(*solution);
        replaceIndividual(worst_index, solution);
        return true;
    }

    int nearest_index = findNearestIndividual(solution);
    if (nearest_index >= 0 && betterForReplacement(solution, population[nearest_index]->solution)) {
        replaceIndividual(nearest_index, solution);
        return true;
    }

    delete solution;
    return false;
}

bool Population::betterForReplacement(Solution *a, Solution *b) {
    if (a == nullptr || !a->feasible) {
        return false;
    }
    if (b == nullptr || !b->feasible) {
        return true;
    }
    if (a->nb_vehicle != b->nb_vehicle) {
        return a->nb_vehicle < b->nb_vehicle;
    }
    return a->cost < b->cost - PRECISION;
}

int Population::findWorstIndividual() {
    int worst_index = 0;
    for (int i = 1; i < population.size(); ++i) {
        if (betterForReplacement(population[worst_index]->solution, population[i]->solution)) {
            worst_index = i;
        }
    }
    return worst_index;
}

int Population::findNearestIndividual(Solution *solution) {
    int nearest_index = -1;
    double nearest_distance = INF;
    for (int i = 0; i < population.size(); ++i) {
        double distance = solution->distance(population[i]->solution);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest_index = i;
        }
    }
    return nearest_index;
}

void Population::replaceIndividual(int index, Solution *solution) {
    if (index < 0 || index >= population.size()) {
        delete solution;
        return;
    }
    delete population[index];
    population[index] = new Individual(solution);
    updateDistances();
}

void Population::updateArchive(Solution *solution) {
    if (solution == nullptr || !solution->feasible) {
        return;
    }
    auto it = archive.find(solution->nb_vehicle);
    if (it == archive.end() || solution->cost < it->second->cost - PRECISION) {
        if (it != archive.end()) {
            delete it->second;
        }
        archive[solution->nb_vehicle] = new Solution(*solution);
    }
}

Solution *Population::selectFromArchive() {
    if (best_solution == nullptr) {
        return nullptr;
    }

    int best_nb_vehicle = best_solution->nb_vehicle;
    auto relaxed_layer = archive.find(best_nb_vehicle + 1);
    if (relaxed_layer != archive.end()) {
        return new Solution(*relaxed_layer->second);
    }

    auto current_layer = archive.find(best_nb_vehicle);
    if (current_layer != archive.end()) {
        return new Solution(*current_layer->second);
    }

    return new Solution(*best_solution);
}

Solution *Population::archiveRouteDescent() {
    Solution *solution = selectFromArchive();
    if (solution == nullptr) {
        auto indices = generator.selectParents(population.size());
        std::vector<Solution*> parents;
        for (auto &index: indices) {
            parents.emplace_back(population[index]->solution);
        }
        solution = generator.breed(parents);
        solution = searcher.FIRD(solution);
        generator.update(parents, solution);
        return solution;
    }

    if (solution->nb_vehicle > data->min_vehicles) {
        solution->decreaseRoute();
    }
    return searcher.FIS(solution, best_solution);
}

void Population::update() {
    double max_cost, min_cost, max_distance, min_distance;
    double coef = params->fit_coef;

    std::sort(population.begin(), population.end(), [](Individual *a, Individual *b) {
        return a->solution->betterThan(b->solution);
    });
    max_cost = population[population.size() - 1]->solution->total_cost;
    min_cost = population[0]->solution->total_cost;

    std::sort(population.begin(), population.end(), [](Individual *a, Individual *b) {
        return a->distance > b->distance;
    });
    max_distance = population[0]->distance;
    min_distance = population[population.size() - 1]->distance;

    for (int i = 0; i < population.size(); ++i) {
        double cost_score = max_cost - min_cost > PRECISION ?
                (max_cost - population[i]->solution->total_cost) / (max_cost - min_cost) : 0;
        double distance_score = max_distance - min_distance > PRECISION ?
                (population[i]->distance - min_distance) / (max_distance - min_distance) : 0;
        population[i]->fitness = cost_score + coef * distance_score;
    }

    std::sort(population.begin(), population.end(), [](Individual *a, Individual *b) {
        return a->fitness > b->fitness;
    });


    while (population.size() > params->population_size) {
        delete population[population.size() - 1];
        population.pop_back();
    }
    updateDistances();
}

void Population::updateDistances() {
    double sum_distance = 0;
    std::vector<double> distances(population.size(), INF);
    for (int i = 0; i < population.size(); ++i) {
        for (int j = i + 1; j < population.size(); ++j) {
            double distance = population[i]->solution->distance(population[j]->solution);
            distances[i] = std::min(distances[i], distance);
            distances[j] = std::min(distances[j], distance);
            sum_distance += 2 * distance;
        }
    }
    for (int i = 0; i < population.size(); ++i) {
        population[i]->distance = distances[i];
    }
}

void Population::print() {
    std::cout << "population : " << std::endl;
    for (auto &ind: population) {
        ind->solution->print();
    }
    std::cout << std::endl;
}
