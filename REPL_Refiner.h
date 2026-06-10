#ifndef REPL_REFINER_H
#define REPL_REFINER_H

#include <memory>
#include "eval_env.h"

void runInteractiveRepl(std::shared_ptr<EvalEnv> env);

#endif
