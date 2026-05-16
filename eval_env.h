#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <unordered_map>
#include "./value.h"

class EvalEnv {
    std::unordered_map<std::string, ValuePtr> symbolTable;

public:
    ValuePtr eval(ValuePtr expr);
    ValuePtr lookup(std::string name);
};

#endif
