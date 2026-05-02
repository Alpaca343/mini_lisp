#include <iomanip>
#include <sstream>
#include <typeinfo>
#include "value.h"

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
//for conv
bool PairValue::isNil(ValuePtr val) const {
    return typeid(*val) == typeid(NilValue);
}

bool PairValue::isPair(ValuePtr val) const {
    return typeid(*val) == typeid(PairValue);
}

std::string PairValue::toString() const {
    std::string result{};
    result += "(";
    result += leftval->toString();
    if (isNil(rightval)) {
        result += ")";
    }
    else if (isPair(rightval)) {
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
    if (isNil(rightval)) {
        result += ")";
    } else if (isPair(rightval)) {
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
