#include "./forms.h"
#include "./eval_env.h"

const std::unordered_map<std::string, SpecialFormType*>& getSpecialForms() {
    static std::unordered_map<std::string, SpecialFormType*> m = {
        {"define", defineForm}, {"quote", quoteForm}, {"if", ifForm}, {"and", andForm}, 
        {"or", orForm},
        {"lambda", lambdaForm},
        {"begin", beginForm},   {"cond", condForm},
        {"let", letForm},
        {"quasiquote", quasiquoteForm},
        {"unquote", unquoteForm}
    };
    return m;
}

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    /* if (args.size() != 2) {
        throw LispError("define requires exactly 2 arguments");
    }*/
    if (auto name = args[0]->asSymbol()) {
        if (args.size() != 2) {
            throw LispError("define requires exactly 2 arguments");
        }
        ValuePtr val = env.eval(args[1]);
        env.define(*name, val);
        return std::make_shared<NilValue>();
    } else if (args[0]->isPair()) { //define lambda
        auto form = std::static_pointer_cast<PairValue>(args[0]);

        if (auto funcName = form->getLeft()->asSymbol()) {
            ValuePtr paramList = form->getRight();
            std::vector<ValuePtr> lambdaItems = {
                std::make_shared<SymbolValue>("lambda"), paramList};
            lambdaItems.insert(lambdaItems.end(), args.begin() + 1, args.end());
            ValuePtr lambdaValue = env.eval(makeList(lambdaItems));
            env.define(*funcName, lambdaValue);
            return std::make_shared<NilValue>();
        } else {
            throw LispError("define: expected function name as symbol");
        }

    } 
    else {
        throw LispError("Unimplemented");
    }
}

ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    return args[0];
}

ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 3) {
        throw LispError("if requires exactly 3 arguments");
    }
    ValuePtr condition = env.eval(args[0]);
    bool isFalse =
        (condition->isBool() &&
         std::static_pointer_cast<BooleanValue>(condition)->getValue() ==
             false);

    if (isFalse) {
        return env.eval(args[2]);
    } else {
        return env.eval(args[1]);
    }
}

ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.empty()) {
        return std::make_shared<BooleanValue>(true);
    }
    ValuePtr condition = nullptr;
    for (auto it = args.begin(); it != args.end(); ++it) {
        condition = env.eval(*it);
        bool isFalse =
            (condition->isBool() &&
             std::static_pointer_cast<BooleanValue>(condition)->getValue() ==
                 false);
        if (isFalse) {
            return std::make_shared<BooleanValue>(false);
        }
    }
    return condition;
}

ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.empty()) {
        return std::make_shared<BooleanValue>(false);
    }
    ValuePtr condition = nullptr;
    for (auto it = args.begin(); it != args.end(); ++it) {
        condition = env.eval(*it);
        bool isFalse =
            (condition->isBool() &&
             std::static_pointer_cast<BooleanValue>(condition)->getValue() ==
                 false);
        if (!isFalse) {
            return condition;
        }
    }
    return std::make_shared<BooleanValue>(false);
}

std::vector<std::string> extractParams(std::vector<ValuePtr> paramVec) {
    std::vector<std::string> params;

    for (const auto& p : paramVec) {
        if (auto name = p->asSymbol()) {
            params.push_back(*name);
        } else {
            throw LispError("lambda: parameter must be a symbol");
        }
    }
    return params;
}

ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("Lambda needs at least 2 arguments");
    }
    if (!args[0]->isPair() && !args[0]->isNil()) {
        throw LispError("The first argument must be a list");
    }

    std::vector<ValuePtr> paramVec;
    if (args[0]->isPair()) {
        paramVec = std::static_pointer_cast<PairValue>(args[0])->toVector();
    }
    std::vector<std::string> params = extractParams(paramVec);
    std::vector<ValuePtr> body;
    for (int i = 1; i < args.size(); ++i) {
        body.push_back(args[i]);
    }

    auto envPtr = env.shared_from_this();
    return make_shared<LambdaValue>(params, body, envPtr);
}

ValuePtr beginForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    ValuePtr condition = nullptr;
    for (auto it = args.begin(); it != args.end(); ++it) {
        condition = env.eval(*it);
    }
    return condition;
}

ValuePtr condForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    for (size_t i = 0; i < args.size(); ++i) {
        auto clause = args[i];
        if (!clause->isPair()) {
            throw LispError("cond: each clause must be a list");
        }

        auto clauseVec =
            std::static_pointer_cast<PairValue>(clause)->toVector();
        if (clauseVec.empty()) {
            throw LispError("cond: empty clause");
        }

        ValuePtr condition = clauseVec[0];
        bool isFalse = false;
        if (auto sym = condition->asSymbol()) {
            if (*sym == "else") {
                if (i != args.size() - 1) {
                    throw LispError("cond: else must be the last clause");
                }
                isFalse = false;
            } else {
                ValuePtr evalCond = env.eval(condition);
                isFalse = (evalCond->isBool() &&
                           std::static_pointer_cast<BooleanValue>(evalCond)
                                   ->getValue() == false);
            }
        } else {
            ValuePtr evalCond = env.eval(condition);
            isFalse =
                (evalCond->isBool() &&
                 std::static_pointer_cast<BooleanValue>(evalCond)->getValue() ==
                     false);
        }

        if (!isFalse) {
            if (clauseVec.size() == 1) {
                return env.eval(condition);
            } else {
                ValuePtr result = nullptr;
                for (size_t j = 1; j < clauseVec.size(); ++j) {
                    result = env.eval(clauseVec[j]);
                }
                return result;
            }
        }
    }

    throw LispError("cond: no true clause");
}

ValuePtr letForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2) {
        throw LispError("let: need at least 2 arguments");
    }

    if (!args[0]->isPair()) {
        throw LispError("let: the first argument must be a lsit");
    }

    std::vector<std::string> params;
    std::vector<ValuePtr> values;

    for (const auto binding : args[0]->toVector()) {
        if (!binding->isPair()) {
            throw LispError("let: the elements in first arg must be a list");
        }
        auto bindingVec = binding->toVector();
        if (bindingVec.size() != 2) {
            throw LispError("let: each binding must have exactly 2 elements");
        }
        if (auto name = bindingVec[0]->asSymbol()) {
            params.push_back(*name);
        } else {
            throw LispError("let: parameter must be a symbol");
        }
        values.push_back(env.eval(bindingVec[1]));
    }
    std::vector<ValuePtr> paramsValue;
    for (auto strval : params) {
        paramsValue.push_back(std::make_shared<SymbolValue>(strval));
    }

    std::vector<ValuePtr> lambdaItems;
    lambdaItems.push_back(std::make_shared<SymbolValue>("lambda"));
    lambdaItems.push_back(makeList(paramsValue));
    for (size_t i = 1; i < args.size(); ++i) {
        lambdaItems.push_back(args[i]);
    }

    auto lambda = env.eval(makeList(lambdaItems));

    std::vector<ValuePtr> lambdaArgs;
    for (const auto& val : values) {
        lambdaArgs.push_back(val);
    }
    auto lambdaPtr = std::static_pointer_cast<LambdaValue>(lambda);
    return lambdaPtr->apply(lambdaArgs);
}

ValuePtr quasiquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 1) {
        throw LispError("quasiquote: exactly 1 argument expected");
    }
    return circleUnquote(args[0], env);
}
//用递归来处理列表中的的unquote
ValuePtr circleUnquote(ValuePtr expr, EvalEnv& env) {
    if (expr->isPair()) {
        auto pairVec = expr->toVector();
        auto pair = std::dynamic_pointer_cast<PairValue>(expr);
        auto first = pair->getLeft();

        if (auto name = first->asSymbol()) {
            if (*name == "unquote") {
                auto rest = pair->getRight();
                if (rest->isNil()) {
                    throw LispError("unquote: expected an expression");
                }
                auto inner =
                    std::dynamic_pointer_cast<PairValue>(rest)->getLeft();
                return env.eval(inner);
            }
        }

        std::vector<ValuePtr> items;
        for (const auto& item : pairVec) {
            items.push_back(circleUnquote(item, env));
        }
        return makeList(items);
    }
    //如果都不是就直接返回原表达式
    return expr;
}

ValuePtr unquoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    throw LispError("unquote: can only be used in quasiquote form");
}
