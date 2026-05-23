#include "./forms.h"
#include "./eval_env.h"

const std::unordered_map<std::string, SpecialFormType*>& getSpecialForms() {
    static std::unordered_map<std::string, SpecialFormType*> m = {
        {"define", defineForm}, {"quote", quoteForm}, {"if", ifForm}, {"and", andForm}, 
        {"or", orForm},       {"lambda", lambdaForm},
    };
    return m;
}

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() != 2) {
        throw LispError("define requires exactly 2 arguments");
    }
    if (auto name = args[0]->asSymbol()) {
        ValuePtr val = env.eval(args[1]);
        env.define(*name, val);
        return std::make_shared<NilValue>();
    } else if (args[0]->isPair()) { //define lambda
        auto form = std::static_pointer_cast<PairValue>(args[0]);

        if (auto funcName = form->getLeft()->asSymbol()) {
            ValuePtr paramList = form->getRight();
            ValuePtr lambdaValue = env.eval(makeList(
                {std::make_shared<SymbolValue>("lambda"), paramList, args[1]}));
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
    if (!args[0]->isPair()) {
        throw LispError("The first argument must be a list");
    }
    auto paramVec = std::static_pointer_cast<PairValue>(args[0])->toVector();
    std::vector<std::string> params = extractParams(paramVec);
    std::vector<ValuePtr> body;
    for (int i = 1; i < args.size(); ++i) {
        body.push_back(args[i]);
    }
    return make_shared<LambdaValue>(params, body);
}
