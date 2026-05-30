#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include "./value.h"
#include "./builtin.h"


class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
    std::unordered_map<std::string, ValuePtr> symbolTable;
    std::shared_ptr<EvalEnv> parent;
    ValuePtr lookup(std::string name);
    EvalEnv();
    explicit EvalEnv(std::shared_ptr<EvalEnv> parent);

public:
    static std::shared_ptr<EvalEnv> createGlobal();

    std::shared_ptr<EvalEnv> createChild(
        const std::vector<std::string>& params,
        const std::vector<ValuePtr>& args);

    ValuePtr eval(ValuePtr expr);
    void define(const std::string& name, ValuePtr value);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr lookupBinding(const std::string& name) const;

};

#endif
