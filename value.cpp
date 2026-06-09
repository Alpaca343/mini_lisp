#include <iomanip>
#include <sstream>
#include <typeinfo>
#include "./value.h"
#include "./eval_env.h"

std::string BooleanValue::toString() const{
    if (value) {
        return "#t";
    } else {
        return "#f";
    }
}

std::string NumericValue::toString() const {
    if (value != (int)value)
    {
        return std::to_string(value);
    }
    else {
        return std::to_string((int)value);
    }
}

std::string StringValue::toString() const {
    std::ostringstream oss;
    oss << std::quoted(value);
    return oss.str();
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return value;
}

std::string PairValue::toString() const {
    std::string result{};
    result += "(";
    result += leftval->toString();
    if (this->rightval->isNil()) {
        result += ")";
    }
    else if (this->rightval->isPair()) {
        result += " ";
        auto& rightPair = static_cast<const PairValue&>(*rightval);
        result += rightPair.toString2();
    } 
    else {
        result += " . ";
        result += rightval->toString();
        result += ")";
    }
    return result;
}
//2string w/o (
std::string PairValue::toString2() const {
    std::string result{};
    result += leftval->toString();
    if (this->rightval->isNil()) {
        result += ")";
    } else if (this->rightval->isPair()) {
        result += " ";
        auto& rightPair = static_cast<const PairValue&>(*rightval);
        result += rightPair.toString2();
    } else {
        result += " . ";
        result += rightval->toString();
        result += ")";
    }
    return result;
}
std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}
std::string LambdaValue::toString() const {
    return "#<procedure>";
}
std::optional<std::string> SymbolValue::asSymbol() const{
    return value;
}
std::optional<double> NumericValue::asNumber() const {
    return value;
}

std::vector<ValuePtr> PairValue::toVector() const {
    std::vector<ValuePtr> list;
    list.push_back(leftval);
    
    if (rightval->isNil()) {
        return list;
    } else if (rightval->isPair()) {
        auto nextList = rightval->toVector();  // 递归调用
        list.insert(list.end(), nextList.begin(), nextList.end());
        return list;
    } else {
        throw LispError("Improper list: dotted pair");
    }
}
std::vector<ValuePtr> NilValue::toVector() const {
    return {};
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args) {
    if (args.size() != params.size()) {
        throw LispError("Lambda: wrong number of arguments");
    }

    auto childEnv = env->createChild(params, args);

    ValuePtr result = nullptr;
    for (const auto& expr : body) {
        result = childEnv->eval(expr);
    }
    return result;
}
