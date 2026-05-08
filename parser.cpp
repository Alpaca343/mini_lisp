#include "./parser.h"
#include "./error.h"

ValuePtr makeList(std::vector<ValuePtr> items) {
    if (items.empty()) {
        return std::make_shared<NilValue>();
    }
    //无情的自动加空列机器人，一个元素的列表也能识别
        items.push_back(std::make_shared<NilValue>());
    while (items.size() > 2) {
        ValuePtr last1 = items.back();
        items.pop_back();
        ValuePtr last2 = items.back();
        items.pop_back();
        items.push_back(std::make_shared<PairValue>(last2, last1));
    }
    return std::make_shared<PairValue>(items[0], items[1]);
}

ValuePtr Parser::parse() {
    if (tokens.empty()) {
        throw SyntaxError("Unexpected end of input");
    }
    auto token = std::move(tokens.front());
    tokens.pop_front();
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    }
    else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    } 
    else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    }
    else if (token->getType() == TokenType::IDENTIFIER) {
        auto& idToken = static_cast<IdentifierToken&>(*token);
        return std::make_shared<SymbolValue>(idToken.getName());
    } 
    else if (token->getType() == TokenType::LEFT_PAREN) {
        return parseTails();
    }
    else if (token->getType() == TokenType::QUOTE) {
        return makeList(
            {std::make_shared<SymbolValue>("quote"), this->parse()});
    } 
    else if (token->getType() == TokenType::QUASIQUOTE) {
        return makeList(
            {std::make_shared<SymbolValue>("quasiquote"), this->parse()});
    } 
    else if (token->getType() == TokenType::UNQUOTE) {
        return makeList(
            {std::make_shared<SymbolValue>("unquote"), this->parse()});
    }
    else {
        throw SyntaxError("Unexpected token");
    }

}

ValuePtr Parser::parseTails() {
    if (tokens.empty()) {
        throw SyntaxError("Expected ')'");
    }
    if (tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = this->parse();
    if (tokens.empty()) {
        throw SyntaxError("Expected ')'");
    }
    if (tokens.front()->getType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        if (tokens.empty() ||
            tokens.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected ')'");
        }
        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}
