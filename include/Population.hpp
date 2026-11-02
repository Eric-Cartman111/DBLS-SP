





#ifndef MA_FIRD_POPULATION_HPP
#define MA_FIRD_POPULATION_HPP


#include <unordered_map>
#include "Config.hpp"
#include "Random.hpp"
#include "Parameters.hpp"
#include "Data.hpp"
#include "Solution.hpp"
#include "Searcher.hpp"
#include "Generator.hpp"

class Individual {
public:
    Solution *solution;
    double distance = 0;
    double fitness = 0;
    Individual(Solution *solution) {
        this->solution = solution;
    }
    ~Individual() {
        delete solution;
    }
};

class Population {
private:
    static Random *random;
    static Parameters *params;
    static Data *data;
    Searcher searcher;
    Generator generator;
    std::unordered_map<int, Solution*> archive;
    bool add(Solution *solution);
    bool addInitial(Solution *solution);
    bool addByFitnessDistance(Solution *solution);
    bool addByQualityNearestNeighbor(Solution *solution);
    bool betterForReplacement(Solution *a, Solution *b);
    int findWorstIndividual();
    int findNearestIndividual(Solution *solution);
    void replaceIndividual(int index, Solution *solution);
    void updateArchive(Solution *solution);
    Solution *selectFromArchive();
    Solution *archiveRouteDescent();
    void update();
    void updateDistances();
public:
    std::vector<Individual*> population {};
    Solution *best_solution = nullptr;
    Population();
    ~Population();
    static void setContext(Random *_random, Parameters *_params, Data *_data);
    void init();
    void next(bool use_archive_guided);
    void print();
};

#endif
