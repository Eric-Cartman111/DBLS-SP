





#ifndef MA_FIRD_OPERATOR_HPP
#define MA_FIRD_OPERATOR_HPP

#include "Solution.hpp"
#include "Evaluation.hpp"

enum Neighborhood {
    RELOCATE_1, RELOCATE_2, RELOCATE_3,
    SWAP_1_1, SWAP_1_2, SWAP_2_2,
    SWAP_1_3, SWAP_2_3, SWAP_3_3,
    TWO_OPT_STAR,
};

class Operator {
private:
    static Random *random;
    static Parameters *params;
    static Data *data;
    double w0 = 1, w1 = 1, w2 = 1;
    double cost_coef = 1, time_coef = 1, load_coef = 1;
    double repair_cost_factor = 0.1;
    double adjust_factor = 0.5;
    double min_pr = 0.1;
    double max_pr = 1e4;
    EvalResult relocate(Solution *solution, int len, double objective_cost);
    EvalResult swap(Solution *solution, int len_i, int len_j, double objective_cost);
    EvalResult twoOptStar(Solution *solution, double objective_cost);
    void _intraRelocate(Solution *solution, Route *route, int len, double objective_cost, EvalResult &res);
    void _intraSwap(Solution *solution, Route *route, int len_i, int len_j, double objective_cost, EvalResult &res);
    void _interRelocate(Solution *solution, Route *ri, Route *rj, int len, double objective_cost, EvalResult &res);
    void _interSwap(Solution *solution, Route *ri, Route *rj, int len_i, int len_j, double objective_cost, EvalResult &res);
    void _twoOptStar(Solution *solution, Route *ri, Route *rj, double objective_cost, EvalResult &res);
    double getScore(double cost_delta, double time_delta, double load_delta) const;
    static bool exceedsObjectiveCost(Solution *solution, double cost_delta, double objective_cost, int removed_routes = 0);
    static bool acceptMove(EvalResult &res) ;
    static bool betterMove(EvalResult &res, double score);
    static bool skip(int rid, int id1, int id2);
public:
    Operator() = default;
    ~Operator() = default;
    static void setContext(Random *_random, Parameters *_params, Data *_data);
    void init(bool flag);
    void updateScoreBalance(Solution *solution, int round, int stagnation, int patience);
    void update(Solution *solution);
    EvalResult operate(Solution *solution, int opt_type, double objective_cost);
    void printCoefs() const;
};

#endif
