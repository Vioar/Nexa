// lexer.hpp

#pragma once

#include <vector>

#include "common/token.hpp"

class Lexer {
private:
    std::string src;
    size_t pos = 0;
    int line = 0;
    int column = 0;

    char peek();
    char peekNext();
    char advance();

    void skipWhitespace();

    Token makeToken(TokenType type, const std::string& text);

    bool startsWith(const std::string& s);

public:
    Lexer(const std::string& text);
    std::vector<Token> tokenize();
};
