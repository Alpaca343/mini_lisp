#include "eval_env.h"

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (auto name = expr->asSymbol()) {
        if (auto value = this->lookup(*name)) {
            return value;
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    } else if (expr->isPair()) {
        using namespace std::literals; //使用 s 后缀
        std::vector<ValuePtr> v = expr->toVector();
        if (v[0]->asSymbol() == "define"s) {
            if (v.size() != 3) {
                throw LispError("define requires exactly 2 arguments");
            }
            if (auto name = v[1]->asSymbol()) {
                ValuePtr val = eval(v[2]);
                symbolTable[*name] = val;
                return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        } else {
            throw LispError("Unimplemented");
        }
    } else {
        throw LispError("Unimplemented");
    }
}

ValuePtr EvalEnv::lookup(std::string name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second;
    }
    return nullptr;
}
