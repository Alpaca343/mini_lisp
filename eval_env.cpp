#include "eval_env.h"
#include "./forms.h"

EvalEnv::EvalEnv() : parent{nullptr} {
    for (const auto& pair : getBuiltins()) {
        symbolTable[pair.first] =
            std::make_shared<BuiltinProcValue>(pair.second);
    }
}
EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent) : parent(std::move(parent)) {}
ValuePtr EvalEnv::lookupBinding(const std::string& name) const {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) {
        return it->second;
    }
    if (parent) {
        return parent->lookupBinding(name);
    }
    return nullptr;
}
ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (auto name = expr->asSymbol()) {
        if (auto value = this->lookupBinding(*name)) {
            return value;
        } else {
            throw LispError("Variable " + *name + " not defined.");
        }
    } else if (expr->isPair()) {
        using namespace std::literals; //使用 s 后缀
        std::vector<ValuePtr> v = expr->toVector();
        if (auto name = v[0]->asSymbol()) {
            const auto& forms = getSpecialForms();
            auto it = forms.find(*name);
            if (it != forms.end()) {
                std::vector<ValuePtr> args(v.begin() + 1, v.end());
                return it->second(args, *this);
            }
        }
        auto calcVal = evalList(expr); //把列表中的每个expr都求值一遍
        if (calcVal.empty()) {
            throw LispError("Empty list");
        }
        ValuePtr proc = calcVal[0];
        std::vector<ValuePtr> args(calcVal.begin() + 1, calcVal.end());
        return apply(proc, args);
    } else {
        throw LispError("Unimplemented");
    }
}

void EvalEnv::define(const std::string& name, ValuePtr value) {
    symbolTable[name] = value;
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
        auto result = fPtr->getFunc()(args , *this);
        return result;
    } else if (auto fPtr = dynamic_pointer_cast<LambdaValue>(proc)) {
        return fPtr->apply(args);
    }
    else{
        throw LispError("Unimplemented");
    }
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(), std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}


std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    return std::shared_ptr<EvalEnv>(new EvalEnv());
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    auto child = std::shared_ptr<EvalEnv>(new EvalEnv(shared_from_this()));
    for (size_t i = 0; i < params.size(); ++i) {
        child->define(params[i], args[i]);
    }
    return child;
}
