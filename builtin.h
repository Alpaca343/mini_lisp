#ifndef BUILTIN_H
#define BUILTIN_H
#include <unordered_map>
#include "./value.h"
#include <cmath>

const std::unordered_map<std::string, BuiltinFuncType*>& getBuiltins();
//calculate
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);
ValuePtr minus(const std::vector<ValuePtr>& params);
ValuePtr multiply(const std::vector<ValuePtr>& params);
ValuePtr divide(const std::vector<ValuePtr>& params);
ValuePtr abs(const std::vector<ValuePtr>& params);
ValuePtr expt(const std::vector<ValuePtr>& params);
ValuePtr quotient(const std::vector<ValuePtr>& params);
ValuePtr remainder(const std::vector<ValuePtr>& params);

// compare
ValuePtr equal(const std::vector<ValuePtr>& params);
ValuePtr greater(const std::vector<ValuePtr>& params);
ValuePtr less(const std::vector<ValuePtr>& params);
ValuePtr greaterEqual(const std::vector<ValuePtr>& params);
ValuePtr lessEqual(const std::vector<ValuePtr>& params);
ValuePtr evenq(const std::vector<ValuePtr>& params);
ValuePtr zeroq(const std::vector<ValuePtr>& params);
ValuePtr oddq(const std::vector<ValuePtr>& params);



#endif  // !BUILTIN_H
