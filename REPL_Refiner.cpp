#include "REPL_Refiner.h"
#include <replxx.hxx>

#include "./tokenizer.h"
#include "./value.h"
#include "./parser.h"
#include "./forms.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace replxx;

// ========== ANSI 支持 ==========
static void enableAnsi() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (GetConsoleMode(hOut, &mode)) {
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}

// ========== 括号/字符串平衡检测 ==========
static bool isBalanced(const std::string& input) {
    int depth = 0;
    bool inString = false;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (inString) {
            if (c == '"') {
                inString = false;
            } else if (c == '\\') {
                ++i;
            }
        } else if (c == ';') {
            while (i < input.size() && input[i] != '\n') ++i;
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

// ========== 输出着色 (ANSI escape codes) ==========
#define COL_RESET   "\033[0m"
#define COL_GREEN   "\033[32m"
#define COL_RED     "\033[31m"
#define COL_YELLOW  "\033[33m"
#define COL_MAGENTA "\033[35m"
#define COL_CYAN    "\033[36m"
#define COL_DIM     "\033[2m"

static std::string colorizeVal(ValuePtr val);
static std::string colorizeRest(ValuePtr val) {
    if (val->isNil()) return ")";
    if (val->isPair()) {
        auto pair = std::static_pointer_cast<PairValue>(val);
        return " " + colorizeVal(pair->getLeft()) + colorizeRest(pair->getRight());
    }
    return " . " + colorizeVal(val) + ")";
}

static std::string colorizeVal(ValuePtr val) {
    if (val->isNumber()) return COL_GREEN + val->toString() + COL_RESET;
    if (val->isString()) return COL_RED + val->toString() + COL_RESET;
    if (val->isBool()) return COL_YELLOW + val->toString() + COL_RESET;
    if (val->isNil()) return COL_DIM "()" COL_RESET;
    if (val->isSymbol()) return COL_CYAN + val->toString() + COL_RESET;
    if (val->isProcedure()) return COL_MAGENTA + val->toString() + COL_RESET;
    if (val->isPair()) {
        auto pair = std::static_pointer_cast<PairValue>(val);
        return "(" + colorizeVal(pair->getLeft()) + colorizeRest(pair->getRight());
    }
    return val->toString();
}

// ========== replxx 输入语法高亮 ==========
// 识别单词
static bool isWordChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) ||
           c == '-' || c == '?' || c == '!' || c == '*' || c == '/' ||
           c == '>' || c == '<' || c == '=' || c == '+';
}

// 复用 getBuiltins / getSpecialForms 来判断单词颜色
static Replxx::Color colorForWord(const std::string& word) {
    if (word == "#t" || word == "#f") return Replxx::Color::YELLOW;

    const auto& forms = getSpecialForms();
    if (forms.find(word) != forms.end()) return Replxx::Color::BRIGHTMAGENTA;

    const auto& builtins = getBuiltins();
    if (builtins.find(word) != builtins.end()) return Replxx::Color::BRIGHTCYAN;

    return Replxx::Color::DEFAULT;
}


// 通过修改replxx_impl.cxx来中的matching_paren改变replxx的默认的括号匹配高亮使其更加符合vs用户的使用习惯
static void highlightCallback(std::string const& input, Replxx::colors_t& colors) {
    size_t i = 0;
    while (i < input.size() && i < colors.size()) {
        char c = input[i];

        // 注释
        if (c == ';') {
            while (i < input.size() && i < colors.size()) {
                colors[i] = Replxx::Color::GRAY;
                if (input[i] == '\n') { ++i; break; }
                ++i;
            }
            continue;
        }

        // 字符串
        if (c == '"') {
            colors[i] = Replxx::Color::BRIGHTRED;
            ++i;
            while (i < input.size() && i < colors.size()) {
                colors[i] = Replxx::Color::BRIGHTRED;
                if (input[i] == '\\') {
                    ++i;
                    if (i < colors.size()) colors[i] = Replxx::Color::BRIGHTRED;
                    ++i;
                    continue;
                }
                if (input[i] == '"') { ++i; break; }
                ++i;
            }
            continue;
        }

        // 括号
        if (c == '(' || c == ')') {
            colors[i] = Replxx::Color::YELLOW;
            ++i;
            continue;
        }

        // 引号
        if (c == '\'' || c == '`' || c == ',') {
            colors[i] = Replxx::Color::BRIGHTCYAN;
            ++i;
            continue;
        }

        // 数字
        if (std::isdigit(static_cast<unsigned char>(c)) ||
            (c == '-' && i + 1 < input.size() &&
             std::isdigit(static_cast<unsigned char>(input[i + 1])))) {
            do {
                colors[i] = Replxx::Color::BRIGHTGREEN;
                ++i;
            } while (i < input.size() &&
                     (std::isdigit(static_cast<unsigned char>(input[i])) ||
                      input[i] == '.'));
            continue;
        }

        // 标识符 — 用 getBuiltins / getSpecialForms 决定颜色
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '+' ||
            c == '-' || c == '*' || c == '/' || c == '>' || c == '<' ||
            c == '=' || c == '?' || c == '!') {
            size_t start = i;
            while (i < input.size() && isWordChar(input[i])) ++i;
            Replxx::Color wc = colorForWord(input.substr(start, i - start));
            for (size_t j = start; j < i; ++j) colors[j] = wc;
            continue;
        }

        colors[i] = Replxx::Color::DEFAULT;
        ++i;
    }
}

// ========== Tab 补全 ==========
static Replxx::completions_t completionCallback(
    std::shared_ptr<EvalEnv> env,
    std::string const& input, int& contextLen) {

    size_t pos = input.find_last_of(" \t\n\r(");
    std::string prefix = (pos == std::string::npos) ? input : input.substr(pos + 1);
    while (!prefix.empty() && (prefix.back() == ')' || prefix.back() == ' ' ||
           prefix.back() == '\t' || prefix.back() == '\n')) {
        prefix.pop_back();
    }

    Replxx::completions_t completions;
    if (prefix.empty()) return completions;
    contextLen = prefix.size();

    // 如果已经完整绑定，不补全
    if (env->lookupBinding(prefix)) return completions;

    // 从 getSpecialForms 补全
    for (const auto& [name, _] : getSpecialForms()) {
        if (name.size() > prefix.size() &&
            name.compare(0, prefix.size(), prefix) == 0) {
            completions.emplace_back(name, Replxx::Color::BRIGHTMAGENTA);
        }
    }

    // 从 getBuiltins 补全
    for (const auto& [name, _] : getBuiltins()) {
        if (name.size() > prefix.size() &&
            name.compare(0, prefix.size(), prefix) == 0) {
            completions.emplace_back(name, Replxx::Color::BRIGHTCYAN);
        }
    }

    // extra 关键字
    static const char* extras[] = {"else", "set!", "delay", "force", "let*", "letrec"};
    for (auto name : extras) {
        std::string s(name);
        if (s.size() > prefix.size() &&
            s.compare(0, prefix.size(), prefix) == 0) {
            completions.emplace_back(s, Replxx::Color::BRIGHTMAGENTA);
        }
    }
    return completions;
}

// ========== 自动缩进计算 ==========
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
    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\n')) pos++;
    while (pos < input.size() && input[pos] != ' ' && input[pos] != '\n' &&
           input[pos] != '(' && input[pos] != ')' && input[pos] != '"' && input[pos] != ';') {
        pos++;
    }
    while (pos < input.size() && (input[pos] == ' ' || input[pos] == '\n')) pos++;

    auto column = [&](size_t p) -> int {
        size_t ls = input.rfind('\n', p);
        return (ls == std::string::npos) ? (int)p : (int)(p - ls - 1);
    };

    if (pos < input.size() && input[pos] != ')') return column(pos);
    return column(lastOpen) + 2;
}

// ========== 交互式 REPL 入口 ==========
void runRepl(std::shared_ptr<EvalEnv> env) {
    enableAnsi();
    Replxx rx;

    rx.set_word_break_characters(" \t\n()\"';,");
    rx.set_double_tab_completion(true);
    rx.set_complete_on_empty(false);

    rx.set_highlighter_callback(highlightCallback);
    rx.set_completion_callback(
        [env](std::string const& input, int& cl) {
            return completionCallback(env, input, cl);
        });

    std::cout << COL_GREEN "Mini-Lisp REPL" COL_RESET
              << "  (Ctrl+D to exit, Tab=complete)\n"
              << std::endl;

    std::string accumulated;
    while (true) {
        std::string prompt = accumulated.empty() ? ">>> " : "... ";
        char const* input = rx.input(prompt + COL_RESET);
        if (input == nullptr) {
            std::cout << std::endl;
            break;
        }

        std::string line(input);
        if (line.empty() && accumulated.empty()) continue;

        accumulated += line;
        accumulated += '\n';

        if (!isBalanced(accumulated)) {
            int indent = calcIndent(accumulated);
            rx.set_preload_buffer(std::string(indent, ' '));
            continue;
        }

        std::string trimmed = accumulated;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        if (trimmed.empty()) { accumulated.clear(); continue; }

        try {
            auto tokens = Tokenizer::tokenize(accumulated);
            Parser parser(std::move(tokens));

            while (true) {
                try {
                    auto value = parser.parse();
                    auto result = env->eval(std::move(value));
                    std::cout << colorizeVal(result) << std::endl;
                } catch (const SyntaxError&) {
                    break;
                } catch (const std::runtime_error& e) {
                    std::cerr << COL_YELLOW "Error: " COL_RESET << e.what() << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << COL_YELLOW "Error: " COL_RESET << e.what() << std::endl;
        }

        accumulated.clear();
    }
}
