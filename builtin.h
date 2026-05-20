#ifndef BUILTIN_H
#define BUILTIN_H
#include <unordered_map>
#include "./value.h"

const std::unordered_map<std::string, BuiltinFuncType*>& getBuiltins();
ValuePtr add(const std::vector<ValuePtr>& params);
ValuePtr print(const std::vector<ValuePtr>& params);



#endif  // !BUILTIN_H
