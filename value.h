#ifndef VALUE_H
#define VALUE_H
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <iostream>
#include "./error.h"

// 你的代码
class EvalEnv; 

class Value {
public:
    Value() = default;
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
    virtual bool isPair() const {
        return false;
    }
    virtual bool isNil() const {
        return false;
    }
    virtual bool isBool() const {
        return false;
    }
    virtual bool isSelfEvaluating() const {
        return false;
    }
    virtual bool isNumber() const {
        return false;
    }
    virtual std::vector<std::shared_ptr<Value>> toVector() const {
        throw LispError("Cannot convert to vec");
    }
    virtual std::optional<std::string> asSymbol() const {
        return std::nullopt;
    }
    virtual std::optional<double> asNumber() const {
        return std::nullopt;
    }
};

using ValuePtr = std::shared_ptr<Value>;

class BooleanValue : public Value {
    bool value;

public:
    BooleanValue(bool value) : value{value} {}
    std::string toString() const;
    bool isSelfEvaluating() const override{
        return true;
    }
    bool isBool() const override{
        return true;
    }
    bool getValue() {
        return value;
    }
};

class NumericValue : public Value {
    double value;

public:
    NumericValue(double value) : value{value} {}
    std::string toString() const;
    bool isSelfEvaluating() const override {
        return true;
    }
    bool isNumber() const override{
        return true;
    }
    std::optional<double> asNumber() const override;
};

class StringValue : public Value {
    std::string value;

public:
    StringValue(std::string value) : value{value} {}
    std::string toString() const;
    bool isSelfEvaluating() const override {
        return true;
    }
};

class NilValue : public Value {

public:
    NilValue() = default;
    std::string toString() const;
    bool isNil() const override{
        return true;
    }
    std::vector<ValuePtr> toVector() const override;
};

class SymbolValue : public Value {
    std::string value;

public:
    SymbolValue(std::string value) : value{value} {}
    std::string toString() const;
    std::optional<std::string> asSymbol() const override;

};

class PairValue : public Value {
    ValuePtr leftval;
    ValuePtr rightval;
public:
    PairValue(ValuePtr left, ValuePtr right)
        : leftval{left}, rightval{right} {}

    std::string toString() const;
    std::string toString2() const;
    bool isPair() const override{
        return true;
    }
    ValuePtr getLeft() const{return leftval;}
    ValuePtr getRight() const{return rightval;}
    std::vector<ValuePtr> toVector() const override;
};

using BuiltinFuncType = ValuePtr(const std::vector<ValuePtr>&);


class BuiltinProcValue : public Value {
    BuiltinFuncType* func;

public:
    
    BuiltinProcValue(BuiltinFuncType* f) : func(f) {}
    std::string toString() const override; // 返回 #<procedure>
    BuiltinFuncType* getFunc() {
        return func;
    }
};

class LambdaValue : public Value {
private:
    std::vector<std::string> params;
    std::vector<ValuePtr> body;
    std::shared_ptr<EvalEnv> env;

public:
    LambdaValue(const std::vector<std::string>& params_, const std::vector<ValuePtr>& body_, const std::shared_ptr<EvalEnv> env_)
        : params(params_), body(body_), env(env_) {}
    std::string toString() const override;  // 返回 #<procedure>
    ValuePtr apply(const std::vector<ValuePtr>& args);
};

#endif
