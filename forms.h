#ifndef FORMS_H
#define FORMS_H

#include <unordered_map>
#include "./value.h"
#include "./parser.h"

class EvalEnv;

using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);

const std::unordered_map<std::string, SpecialFormType*>& getSpecialForms();

std::vector<std::string> extractParams(ValuePtr paramList);

ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env);
ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env);

#endif
