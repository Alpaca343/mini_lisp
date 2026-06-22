#include "./builtin.h"
#include "./eval_env.h"

const std::unordered_map<std::string, BuiltinFuncType*>& getBuiltins() {
    static std::unordered_map<std::string, BuiltinFuncType*> m =
    {  // 用static避免反复创建新的临时变量，同时避免悬垂引用
        {"print", print}, {"+", add}, {"-", minus}, {"*", multiply},
        {"/", divide},
        {"abs", abs},
        {"expt", expt},
        {"quotient", quotient},
        {"remainder", remainder},
        {"=", equal},
        {">", greater},
        {"<", less},
        {">=", greaterEqual},
        {"<=", lessEqual},
        {"even?", evenq},
        {"odd?", oddq},
        {"zero?", zeroq},
        {"car", car},
        {"cdr", cdr},
        {"eq?", eqq},
        {"equal?", equalq},
        {"not", notf},
        {"modulo", modulo},
        {"append", append},
        {"cons", cons},
        {"length", length},
        {"list", list},
        {"map", map},
        {"filter", filter},
        {"reduce", reducef},
        {"apply", applyf},
        {"display", display},
        {"displayln", displayln},
        {"eval", eval},
        {"error", error},
        {"exit", exit},
        {"newline", newline},
        {"atom?", atomQ},
        {"boolean?", booleanQ},
        {"integer?", integerQ},
        {"list?", listQ},
        {"number?", numberQ},
        {"null?", nullQ},
        {"pair?", pairQ},
        {"procedure?", procedureQ},
        {"string?", stringQ},
        {"symbol?", symbolQ},
    };
    return m;
}

ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (const auto& i : params) {
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}
//核心库
ValuePtr applyf(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("apply: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    auto proc = params[0];
    auto list = params[1];
    if (list->isPair() || list->isNil()) {
        std::vector<ValuePtr> args = list->toVector();
        return env.apply(proc, args);
    } else {
        throw LispError("apply: the second argument should be a list.");
    }

}
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    ValuePtr val = params[0];
    std::string str = val->toString();
    if (val->isString()) {
        if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
                str = str.substr(1, str.size() - 2);
        }
    }
    std::cout << str;
    return std::make_shared<NilValue>();
}
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    ValuePtr val = params[0];
    std::string str = val->toString();
    if (val->isString()) {
        if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
            str = str.substr(1, str.size() - 2);
        }
    }
    std::cout << str << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("error: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    ValuePtr val = params[0];
    throw LispError(val->toString());
}
ValuePtr exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    int exitCode = 0; 
    if (params.size() == 1) {
        ValuePtr val = params[0];
        if (auto num = val->asNumber()) {
            exitCode = static_cast<int>(*num);
        } else {
            throw LispError("exit: expected a number");
        }
    } else if (params.size() > 1) {
        throw LispError(
            "exit: wrong number of arguments (expected 0 or 1, got " +
            std::to_string(params.size()) + ")");
    }

    std::exit(exitCode);
}
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("eval: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    return env.eval(params[0]);
}
//类型检查库
ValuePtr atomQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("atom?: wrong number of arguments");
    }
    ValuePtr arg = params[0]; 
    bool result = (arg->isBool() || arg->isNumber() || arg->isString() ||
                   arg->isSymbol() || arg->isNil());
    return std::make_shared<BooleanValue>(result);
}

ValuePtr booleanQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("boolean?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isBool());
}

ValuePtr integerQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("integer?: wrong number of arguments");
    }
    ValuePtr arg = params[0];
    bool result = false;
    if (auto num = arg->asNumber()) {
        result = (std::fmod(*num, 1.0) == 0);
    }
    return std::make_shared<BooleanValue>(result);
}

ValuePtr listQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("list?: wrong number of arguments");
    }
    ValuePtr arg = params[0];

    if (arg->isNil()) {
        return std::make_shared<BooleanValue>(true);
    }

    bool result = false;
    if (arg->isPair()) {
        try {
            arg->toVector(); 
            result = true;
        } catch (LispError) {
            result = false;
        }
    }
    return std::make_shared<BooleanValue>(result);
}

ValuePtr numberQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("number?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isNumber());
}

ValuePtr nullQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("null?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isNil());
}

ValuePtr pairQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("pair?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isPair());
}

ValuePtr procedureQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("procedure?: wrong number of arguments");
    }
    ValuePtr arg = params[0];
    bool result = (dynamic_pointer_cast<BuiltinProcValue>(arg) != nullptr ||
                   dynamic_pointer_cast<LambdaValue>(arg) != nullptr);
    return std::make_shared<BooleanValue>(result);
}

ValuePtr stringQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("string?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isString());
}

ValuePtr symbolQ(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("symbol?: wrong number of arguments");
    }
    return std::make_shared<BooleanValue>(params[0]->isSymbol());
}
//算数运算库

ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto result = 0.0;
    for (const auto& i : params) {
        if (auto num = i->asNumber()) {
            result += *num;
        } else {
            throw LispError("Cannot add a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr minus(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        throw LispError("minus: wrong number of arguments (expected at least 1, got 0)");
    }
    auto x = params[0]->asNumber();
    if (!x) throw LispError("minus: expected number");

    if (params.size() == 1) {
        return std::make_shared<NumericValue>(-(*x));
    }

    auto result = *x;
    for (size_t i = 1; i < params.size(); ++i) {
        if (auto num = params[i]->asNumber()) {
            result -= *num;
        } else {
            throw LispError("minus: expected number");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto result = 1.0;
    for (const auto& i : params) {
        if (auto num = i->asNumber()) {
            result *= *num;
        } else {
            throw LispError("Cannot multiply a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto result = 1.0;
    if (params.size() < 1) {
        throw LispError("divide: wrong number of arguments (expected 1 or 2, got " +
                        std::to_string(params.size()) + ")");
    } else if (params.size() > 2) {
        throw LispError("divide: wrong number of arguments (expected 1 or 2, got " +
                        std::to_string(params.size()) + ")");
    } else if (params.size() == 1) {
        auto num = params[0]->asNumber();
        if (!num) {
            throw LispError(
                "Division: expected number, got non-number");
        }
        result = 1.0 / (*num);
    } else {  // params.size() == 2
        auto numerator = params[0]->asNumber();
        auto denominator = params[1]->asNumber();
        if (!numerator) {
            throw LispError(
                "Division: expected number, got non-number");
        }
        if (!denominator) {
            throw LispError(
                "Division: expected number, got non-number");
        }
        if (*denominator == 0) {
            throw LispError("Division by zero");
        }
        result = (*numerator) / (*denominator);
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr abs(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("abs: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    if(auto num = params[0]->asNumber()) {
        if (*num >= 0) {
            return std::make_shared<NumericValue>(*num);
        } else {
            return std::make_shared<NumericValue>(-*num);
        }
    } else {
        throw LispError("abs: expected a number");
    }
}

ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("expt: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    auto baseOpt = params[0]->asNumber();
    auto expOpt = params[1]->asNumber();

    if (!baseOpt) {
        throw LispError("expt: expected number as first argument");
    }
    if (!expOpt) {
        throw LispError("expt: expected number as second argument");
    }

    double base = *baseOpt;
    double exponent = *expOpt;

    // 特殊情况：0^0 = 1 （或者行为未定义扔个错误，可以问一下）
    if (base == 0.0 && exponent == 0.0) {
        return std::make_shared<NumericValue>(1.0);
    }

    double result = std::pow(base, exponent);
    return std::make_shared<NumericValue>(result);
}

ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("quotient: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }
    auto x = params[0]->asNumber();
    auto y = params[1]->asNumber();

    if (!x) throw LispError("quotient: expected number");
    if (!y) throw LispError("quotient: expected number");
    if (*y == 0) throw LispError("quotient: division by zero");

    int result = static_cast<int>(*x / *y);
    return std::make_shared<NumericValue>(result);
}

ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("remainder: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }
    auto x = params[0]->asNumber();
    auto y = params[1]->asNumber();

    if (!x) throw LispError("remainder: expected number");
    if (!y) throw LispError("remainder: expected number");
    if (*y == 0) throw LispError("remainder: division by zero");

    int quotient_result = static_cast<int>(*x / *y);
    double result = *x - *y * quotient_result;
    return std::make_shared<NumericValue>(result);
}

ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("modulo: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }
    auto xOpt = params[0]->asNumber();
    auto yOpt = params[1]->asNumber();
    if (!xOpt) throw LispError("modulo: expected number");
    if (!yOpt) throw LispError("modulo: expected number");
    if (*yOpt == 0) throw LispError("modulo: division by zero");
    double x = *xOpt;
    double y = *yOpt;
    double remainder = std::fmod(x, y);
    if (remainder != 0 &&
        ((y > 0 && remainder < 0) || (y < 0 && remainder > 0))) {
        remainder += y;
    }
    return std::make_shared<NumericValue>(remainder);
}

//比较库（使用模板函数来比较）
template <typename Op>
ValuePtr compare(const std::vector<ValuePtr>& params, const std::string& name,
                 Op op) {
    if (params.size() != 2) {
        throw LispError(name + ": wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    auto x = params[0]->asNumber();
    auto y = params[1]->asNumber();

    if (!x) throw LispError(name + ": expected number");
    if (!y) throw LispError(name + ": expected number");

    bool result = op(*x, *y);
    return std::make_shared<BooleanValue>(result);
}

ValuePtr equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return compare(params, "=", std::equal_to<double>());
}

ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return compare(params, ">", std::greater<double>());
}

ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return compare(params, "<", std::less<double>());
}

ValuePtr greaterEqual(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return compare(params, ">=", std::greater_equal<double>());
}

ValuePtr lessEqual(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return compare(params, "<=", std::less_equal<double>());
}

template <typename Pred>
ValuePtr unaryPredicate(const std::vector<ValuePtr>& params,
                        const std::string& name, Pred pred) {
    if (params.size() != 1) {
        throw LispError(name + ": wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }

    auto x = params[0]->asNumber();
    if (!x) {
        throw LispError(name + ": expected number");
    }

    bool result = pred(*x);
    return std::make_shared<BooleanValue>(result);
}

ValuePtr evenq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return unaryPredicate(params, "even?", [](double x) {
        return std::fmod(x, 1.0) == 0 && static_cast<int>(x) % 2 == 0; //fmod是支持浮点数的求余
    });
}

ValuePtr oddq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return unaryPredicate(params, "odd?", [](double x) {
        return std::fmod(x, 1.0) == 0 && static_cast<int>(x) % 2 != 0;
    });
}

ValuePtr zeroq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return unaryPredicate(params, "zero?", [](double x) { return x == 0.0; });
}

static bool isEqualValue(ValuePtr a, ValuePtr b);

ValuePtr equalq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("equal?: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    ValuePtr a = params[0];
    ValuePtr b = params[1];

    return std::make_shared<BooleanValue>(isEqualValue(a, b));
}

static bool isEqualValue(ValuePtr a, ValuePtr b) {
    if (a == b) return true;
    if (a->isNumber() && b->isNumber()) {
        return *a->asNumber() == *b->asNumber();
    }
    if (a->isBool() && b->isBool()) {
        return std::static_pointer_cast<BooleanValue>(a)->getValue() ==
               std::static_pointer_cast<BooleanValue>(b)->getValue();
    }
    if (a->isSymbol() && b->isSymbol()) {
        return *a->asSymbol() == *b->asSymbol();
    }
    if (a->isNil() && b->isNil()) return true;
    if (a->isString() && b->isString()) {
        return a->toString() == b->toString();
    }
    if (a->isPair() && b->isPair()) {
        auto aPair = std::static_pointer_cast<PairValue>(a);
        auto bPair = std::static_pointer_cast<PairValue>(b);
        return isEqualValue(aPair->getLeft(), bPair->getLeft()) &&
               isEqualValue(aPair->getRight(), bPair->getRight());
    }
    if (a->isProcedure() && b->isProcedure()) {
        return a == b;
    }

    return false;
}

ValuePtr eqq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("eq?: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    ValuePtr a = params[0];
    ValuePtr b = params[1];
    if (a == b) {
        return std::make_shared<BooleanValue>(true);
    }

    if (a->isNumber() && b->isNumber()) {
        double aNum = *a->asNumber();
        double bNum = *b->asNumber();
        return std::make_shared<BooleanValue>(aNum == bNum);
    }
    if (a->isBool() && b->isBool()) {
        bool aBool = std::static_pointer_cast<BooleanValue>(a)->getValue();
        bool bBool = std::static_pointer_cast<BooleanValue>(b)->getValue();
        return std::make_shared<BooleanValue>(aBool == bBool);
    }
    if (a->isSymbol() && b->isSymbol()) {
        std::string aSym = *a->asSymbol();
        std::string bSym = *b->asSymbol();
        return std::make_shared<BooleanValue>(aSym == bSym);
    }
    if (a->isNil() && b->isNil()) {
        return std::make_shared<BooleanValue>(true);
    }
    return std::make_shared<BooleanValue>(false);
}

ValuePtr notf(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("not: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    ValuePtr val = params[0];
    bool isFalse =
        (val->isBool() &&
         std::static_pointer_cast<BooleanValue>(val)->getValue() == false);

    return std::make_shared<BooleanValue>(isFalse);
}
// 对子与列表操作库
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("car: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    if (auto pair = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pair->getLeft();
    } else {
        throw LispError("car: expected a pair");
    }
}

ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("cdr: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }
    if (auto pair = std::dynamic_pointer_cast<PairValue>(params[0])) {
        return pair->getRight();
    } else {
        throw LispError("cdr: expected a pair");
    }
}

ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        return std::make_shared<NilValue>();
    }

    std::vector<ValuePtr> result;
    for (const auto& lst : params) {
        if (lst->isNil()) {
            continue;
        } else if (lst->isPair()) {
            auto vec = lst->toVector();
            result.insert(result.end(), vec.begin(), vec.end());
        } else {
            throw LispError("append: expected list");
        }
    }
    return makeList(result);
}

ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) {
        throw LispError("length: wrong number of arguments (expected 1, got " +
                        std::to_string(params.size()) + ")");
    }

    ValuePtr arg = params[0];

    if (arg->isNil()) {
        return std::make_shared<NumericValue>(0);
    }

    if (!arg->isPair()) {
        throw LispError("length: expected a list");
    }

    size_t len = arg->toVector().size();
    return std::make_shared<NumericValue>(static_cast<double>(len));
}

ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 2) {
        return std::make_shared<PairValue>(params[0], params[1]);
    } else {
        throw LispError("cons: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }
}

ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        return std::make_shared<NilValue>();
    } 
    return makeList(params);
}

ValuePtr map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("map: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    ValuePtr proc = params[0];
    ValuePtr list = params[1];
    if (!list->isPair() && !list->isNil()) {
        throw LispError("map: expected a list");
    }
    std::vector<ValuePtr> result;
    std::vector<ValuePtr> elements = list->toVector();
    for (const auto& elem : elements) {
        auto res = env.apply(proc, {elem});
        result.push_back(res);
    }
    return makeList(result);
}

ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("filter: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }
    ValuePtr proc = params[0];
    ValuePtr list = params[1];
    if (!list->isPair() && !list->isNil()) {
        throw LispError("filter: expected a list");
    }
    std::vector<ValuePtr> result;
    std::vector<ValuePtr> elements = list->toVector();
    
    for (const auto& elem : elements) {
        std::vector<ValuePtr> args = {elem};
        ValuePtr val = env.apply(proc, args);
        bool isFalse =
            (val->isBool() &&
             std::static_pointer_cast<BooleanValue>(val)->getValue() == false);
        if (!isFalse) {
            result.push_back(elem);
        }
    }
    return makeList(result);
}
ValuePtr reducef(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) {
        throw LispError("reduce: wrong number of arguments (expected 2, got " +
                        std::to_string(params.size()) + ")");
    }

    ValuePtr proc = params[0];
    ValuePtr list = params[1];

    if (list->isNil()) {
        throw LispError("reduce: list cannot be empty");
    }
    if (!list->isPair()) {
        throw LispError("reduce: expected a list");
    }

    std::vector<ValuePtr> elements = list->toVector();

    if (elements.size() == 1) {
        return elements[0];
    }

    ValuePtr result = elements[0];
    for (size_t i = 1; i < elements.size(); ++i) {
        result = env.apply(proc, {result, elements[i]});
    }

    return result;
}
