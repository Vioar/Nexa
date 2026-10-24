// parser.hpp

#pragma once

#include <vector>
#include <memory>

#include "common/token.hpp"
#include "ast/ast.hpp"
#include "parser/precedence.hpp"

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;
    bool replMode;

    Token& peek();
    Token& advance();
    bool match(TokenType type);

    std::unique_ptr<Expr> parseExpression(Precedence prec);

    std::unique_ptr<Expr> parsePrefix(const Token& t);
    std::unique_ptr<Expr> parseInfix(std::unique_ptr<Expr> left, const Token& op);

    std::unique_ptr<Expr> parseList();

    std::unique_ptr<Stmt> parseStatement();

    std::vector<std::string> parseIdentifierList();
    std::vector<std::unique_ptr<Expr>> parseExpressionList(Precedence prec);

public:
    Parser(const std::vector<Token>& tokens, bool repl);
    std::vector<std::unique_ptr<Stmt>> parse();
};
