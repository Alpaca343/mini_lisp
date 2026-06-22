#ifndef BUILTIN_H
#define BUILTIN_H
#include <unordered_map>
#include "./value.h"
#include "./parser.h"
#include <cmath>
#include <vector>
#include <cstdlib>

class EvalEnv;
const std::unordered_map<std::string, BuiltinFuncType*>& getBuiltins();
//core
ValuePtr applyf(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr exit(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env);

//typecheck
ValuePtr atomQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr booleanQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr integerQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr listQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr numberQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr nullQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr pairQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr procedureQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr stringQ(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr symbolQ(const std::vector<ValuePtr>& params, EvalEnv& env);

//calculate
ValuePtr add(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr minus(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr abs(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env);

// compare
ValuePtr equal(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr eqq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr equalq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr greaterEqual(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr lessEqual(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr evenq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr zeroq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr oddq(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr notf(const std::vector<ValuePtr>& params, EvalEnv& env);

//pair and list
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr map(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env);
ValuePtr reducef(const std::vector<ValuePtr>& params, EvalEnv& env);
#endif  // !BUILTIN_H
