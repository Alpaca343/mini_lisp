#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "./tokenizer.h"
#include "./value.h"
#include "./parser.h"
#include "./eval_env.h"
#include "REPL_Refiner.h"
#include "rjsj_test.hpp"

struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};

static void evalf(EvalEnv& env, Parser& parser) {
    while (true) {
        try {
            auto value = parser.parse();
            auto result = env.eval(std::move(value));
            (void)result;  // 文件模式不输出
        } catch (const SyntaxError&) {
            break;
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    //RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    if (argc == 1) {
        runInteractiveRepl(EvalEnv::createGlobal());
    } else if (argc == 2) {
        try {
            std::string filename = argv[1];
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Cannot open file: " << filename << std::endl;
                return 1;
            }
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

            auto tokens = Tokenizer::tokenize(content);
            Parser parser(std::move(tokens));
            auto env = EvalEnv::createGlobal();
            evalf(*env, parser);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: too many arguments" << std::endl;
    }
}
