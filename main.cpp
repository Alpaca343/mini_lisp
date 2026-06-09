#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "./tokenizer.h"
#include "./value.h"
#include "./parser.h"
#include "./eval_env.h"
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
//检查括号和字符串是否闭合，用于判断多行输入是否完成
static bool isBalanced(const std::string& input) {
    int depth = 0;
    bool inString = false;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inString) {
            if (c == '"') {
                inString = false;
            }
        } else if (c == ';') {
            break;
        } else if (c == '"') {
            inString = true;
        } else if (c == '(') {
            depth++;
        } else if (c == ')') {
            depth--;
        }
    }
    return depth <= 0 && !inString;
}

//计算缩进位置：对齐到最后一个未闭合括号内的第一个参数列
static int calcIndent(const std::string& input) {
    std::vector<size_t> stack;
    bool inString = false;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inString) {
            if (c == '"') inString = false;
        } else if (c == ';') {
            while (i < input.size() && input[i] != '\n') i++;
        } else if (c == '"') {
            inString = true;
        } else if (c == '(') {
            stack.push_back(i);
        } else if (c == ')' && !stack.empty()) {
            stack.pop_back();
        }
    }
    if (stack.empty()) return 0;

    size_t lastOpen = stack.back();
    size_t pos = lastOpen + 1;
    // 跳过运算符名（第一个符号），定位到第一个参数
    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\n')) pos++;
    while (pos < input.size() && input[pos] != ' ' && input[pos] != '\n' &&
           input[pos] != '(' && input[pos] != ')' && input[pos] != '"' && input[pos] != ';') {
        pos++;
    }
    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\n')) pos++;

    // 将绝对位置转换为当前行内的列数
    auto column = [&](size_t p) -> int {
        size_t ls = input.rfind('\n', p);
        return (ls == std::string::npos) ? (int)p : (int)(p - ls - 1);
    };

    if (pos < input.size() && input[pos] != ')') return column(pos);
    return column(lastOpen) + 2;  // 没有参数时从 ( 缩进 2 格
}

//用来执行两种模式下都要进行的操作
static void evalf(EvalEnv& env, Parser& parser, bool isPrint) {
    while (true) {
        try {
            auto value = parser.parse();
            auto result = env.eval(std::move(value));
            if (isPrint) {
                std::cout << result->toString() << std::endl;
            }
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
        auto env = EvalEnv::createGlobal();
        std::string accumulated;
        while (true) {
            if (accumulated.empty()) {
                std::cout << ">>> ";
            } else {  //判断缩进大小
                int indent = calcIndent(accumulated);
                std::cout << "... " << std::string(indent, ' ');
            }
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                break;
            }
            accumulated += line + "\n";
            if (!isBalanced(accumulated)) {
                continue;
            }
            try {
                auto tokens = Tokenizer::tokenize(accumulated);
                Parser parser(std::move(tokens));
                evalf(*env, parser, true);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            accumulated.clear();
        }
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
            evalf(*env, parser, false);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Error: too many arguments" << std::endl;
    }
}
