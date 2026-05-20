#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include "./value.h"
#include "./builtin.h"

class EvalEnv {
    std::unordered_map<std::string, ValuePtr> symbolTable;

public:
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    ValuePtr lookup(std::string name);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    std::vector<ValuePtr> evalList(ValuePtr expr);
};

#endif
