#ifndef PARSER_H
#define PARSER_H
#include <deque>
#include <vector>
#include "./token.h"
#include "./value.h" 

class Parser {
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens) : tokens{std::move(tokens)} {}
    ValuePtr parse();
    ValuePtr parseTails();
    
};

ValuePtr makeList(std::vector<ValuePtr> items);


#endif  // !PARSER_H
