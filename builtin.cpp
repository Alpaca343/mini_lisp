#include "./builtin.h"

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
    };
    return m;
}

ValuePtr print(const std::vector<ValuePtr>& params) {
    for (const auto& i : params) {
        std::cout << i->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

//算数运算库

ValuePtr add(const std::vector<ValuePtr>& params) {
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

ValuePtr minus(const std::vector<ValuePtr>& params) {
    auto result = 2.0 * *(params[0]->asNumber());
    for (const auto& i : params) {
        if (auto num = i->asNumber()) {
            result -= *num;
        } else {
            throw LispError("Cannot minus a non-numeric value.");
        }
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr multiply(const std::vector<ValuePtr>& params) {
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

ValuePtr divide(const std::vector<ValuePtr>& params) {
    auto result = 1.0;
    if (params.size() < 1) {
        throw LispError("Too few arguments: " +
                        std::to_string(params.size()) + " < 1");
    } else if (params.size() > 2) {
        throw LispError("Too many arguments: " +
                        std::to_string(params.size()) + " > 2");
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

ValuePtr abs(const std::vector<ValuePtr>& params) {
    if (params.size() == 1) {
        if(auto num = params[0]->asNumber()) {
            if (*num >= 0) {
                return std::make_shared<NumericValue>(*num);
            } else {
                return std::make_shared<NumericValue>(-*num);
            }
        } else {
            throw LispError("Expect a number");
        }
    } else {
        throw LispError("Expect 1 argument, given " +
                        std::to_string(params.size()) + " arguments");
    }
}

ValuePtr expt(const std::vector<ValuePtr>& params) {
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

ValuePtr quotient(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("expt: wrong number of arguments (expected 2, got " +
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

ValuePtr remainder(const std::vector<ValuePtr>& params) {
    if (params.size() != 2) {
        throw LispError("expt: wrong number of arguments (expected 2, got " +
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

ValuePtr equal(const std::vector<ValuePtr>& params) {
    return compare(params, "=", std::equal_to<double>());
}

ValuePtr greater(const std::vector<ValuePtr>& params) {
    return compare(params, ">", std::greater<double>());
}

ValuePtr less(const std::vector<ValuePtr>& params) {
    return compare(params, "<", std::less<double>());
}

ValuePtr greaterEqual(const std::vector<ValuePtr>& params) {
    return compare(params, ">=", std::greater_equal<double>());
}

ValuePtr lessEqual(const std::vector<ValuePtr>& params) {
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

ValuePtr evenq(const std::vector<ValuePtr>& params) {
    return unaryPredicate(params, "even?", [](double x) {
        return std::fmod(x, 1.0) == 0 && static_cast<int>(x) % 2 == 0; //fmod是支持浮点数的求余
    });
}

ValuePtr oddq(const std::vector<ValuePtr>& params) {
    return unaryPredicate(params, "odd?", [](double x) {
        return std::fmod(x, 1.0) == 0 && static_cast<int>(x) % 2 != 0;
    });
}

ValuePtr zeroq(const std::vector<ValuePtr>& params) {
    return unaryPredicate(params, "zero?", [](double x) { return x == 0.0; });
}

// 对子与列表操作库
ValuePtr car(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("car: expect 1 argument");
    }
    if (auto pair = static_pointer_cast<PairValue>(params[0])) {
        return pair->getLeft();
    } else {
        throw LispError("expect a pair");
    }
}

ValuePtr cdr(const std::vector<ValuePtr>& params) {
    if (params.size() != 1) {
        throw LispError("car: expect 1 argument");
    }
    if (auto pair = static_pointer_cast<PairValue>(params[0])) {
        return pair->getRight();
    } else {
        throw LispError("expect a pair");
    }
}
