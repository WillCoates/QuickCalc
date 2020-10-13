#include "concepts.hpp"
#include <unordered_map>
#include <cmath>

using namespace quickcalc;

namespace {
    constexpr double QC_FALSE = 0.0;
    constexpr double QC_TRUE = 1.0;
    constexpr double QC_EPSILON = 1e-15;
    // C++17 doesn't define a pi constant, so we'll use our own
    constexpr double QC_PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164063;
    
    double qcIf(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        double param0 = exec.evaluate(params[0].get());

        // false
        if (abs(param0) < QC_EPSILON) {
            if (params.size() >= 3) {
                return exec.evaluate(params[2].get());
            } else {
                return param0;
            }
        } else {
            return exec.evaluate(params[1].get());
        }
    }

    double qcEq(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return abs(exec.evaluate(params[0].get()) - exec.evaluate(params[1].get())) < QC_EPSILON;
    }

    double qcNe(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return abs(exec.evaluate(params[0].get()) - exec.evaluate(params[1].get())) >= QC_EPSILON;
    }

    double qcGt(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return exec.evaluate(params[0].get()) > exec.evaluate(params[1].get());
    }

    double qcLt(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return exec.evaluate(params[0].get()) < exec.evaluate(params[1].get());
    }

    double qcGe(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return exec.evaluate(params[0].get()) >= exec.evaluate(params[1].get());
    }

    double qcLe(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        if (params.size() < 2) {
            return NAN;
        }
        return exec.evaluate(params[0].get()) <= exec.evaluate(params[1].get());
    }

    double qcTrue(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        return QC_TRUE;
    }
    
    double qcFalse(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        return QC_FALSE;
    }

    double qcEpsilon(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        return QC_EPSILON;
    }

    double qcPi(Executor &exec, const std::vector<ExprNode::ptr> &params) {
        return QC_PI;
    }

    std::unordered_map<std::string, ExecutorState::Func> functions = {
        { "if", &qcIf },
        { "eq", &qcEq },
        { "ne", &qcNe },
        { "gt", &qcGt },
        { "lt", &qcLt },
        { "ge", &qcGe },
        { "le", &qcLe },
        { "TRUE", &qcTrue },
        { "true", &qcTrue },
        { "FALSE", &qcFalse },
        { "false", &qcFalse },
        { "EPSILON", &qcEpsilon },
        { "PI", &qcPi },
    };
}

void quickcalc::loadConcepts(ExecutorState &state) {
    for (auto &func : functions) {
        state.setFunction(func.first, func.second);
    }
}
