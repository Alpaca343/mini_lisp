#include "eval_env.h"

EvalEnv::EvalEnv() {
    for (const auto& pair : getBuiltins()) {
        symbolTable[pair.first] =
            std::make_shared<BuiltinProcValue>(pair.second);
    }
}

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
            auto calcVal = evalList(expr); //把列表中的每个expr都求值一遍
            if (calcVal.empty()) {
                throw LispError("Empty list");
            }
            ValuePtr proc = calcVal[0];
            std::vector<ValuePtr> args(calcVal.begin() + 1, calcVal.end());
            return apply(proc, args);
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

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (auto fPtr = dynamic_pointer_cast<BuiltinProcValue>(proc)) {
        auto result = fPtr->getFunc()(args);
        return result;
    } else {
        throw LispError("Unimplemented");
    }
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(), std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
