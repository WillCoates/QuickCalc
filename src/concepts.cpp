#include "concepts.hpp"
#include <unordered_map>

using namespace quickcalc;

namespace {
    double qcIf(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        double param0 = exec.evaluate(params[0].get());

        // false
        if (abs(param0) < 0.00001) {
            if (params.size() >= 3) {
                return exec.evaluate(params[2].get());
            } else {
                return param0;
            }
        } else {
            return exec.evaluate(params[1].get());
        }
    }

    std::unordered_map<std::string, ExecutorState::Func> functions = {
        { "if", &qcIf },
    };
}

void quickcalc::loadConcepts(ExecutorState &state) {
    for (auto &func : functions) {
        state.setFunction(func.first, func.second);
    }
}
