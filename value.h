#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <memory>


// 你的代码
class Value {
public:
    Value() = default;
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
};

using ValuePtr = std::shared_ptr<Value>;

class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : value{value} {}
    std::string toString() const;
};

class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : value{value} {}
    std::string toString() const;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(std::string value) : value{value} {}
    std::string toString() const;
};

class NilValue : public Value {

public:
    NilValue() = default;
    std::string toString() const;
};

class SymbolValue : public Value {
    std::string value;

public:
    SymbolValue(std::string value) : value{value} {}
    std::string toString() const;
};

class PairValue : public Value {
    ValuePtr leftval;
    ValuePtr rightval;


public:
    PairValue(ValuePtr left, ValuePtr right)
        : leftval{left}, rightval{right} {}
    bool isNil(ValuePtr val) const;
    bool isPair(ValuePtr val) const;
    std::string toString() const;
    std::string toString2() const;
};


#endif
