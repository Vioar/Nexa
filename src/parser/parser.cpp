// parser.cpp

#include <memory>

#include "parser/parser.hpp"
#include "ast/ast.hpp"
#include "common/token.hpp"
#include "parser/parser_utils.hpp"
#include "common/error.hpp"
#include "parser/precedence.hpp"
#include "common/error.hpp"
#include "common/types.hpp"

Parser::Parser(const std::vector<Token>& t, bool repl)
: tokens(t), replMode(repl) {}

Token& Parser::peek() {
    return tokens[pos];
}

Token& Parser::advance() {
    return tokens[pos++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        pos++;
        return true;
    }
    return false;
}

std::unique_ptr<Expr> Parser::parsePrefix(const Token& t) {
    switch (t.type) {
        case TokenType::Minus: {
            return std::make_unique<UnaryOp>(
                tokenToOp(t.type),
                parseExpression(Precedence::Prefix),
                SourceLocation{t.line, t.column}
            );
        }
        case TokenType::Number: {
            if (t.text.find('.') != std::string::npos) {
                return std::make_unique<FloatExpr>(
                    std::stod(t.text),
                    SourceLocation{t.line, t.column}
                );
            }
            return std::make_unique<NumberExpr>(
                std::stoi(t.text),
                SourceLocation{t.line, t.column}
            );
        }

        case TokenType::StringLiteral: {
            return std::make_unique<StringExpr>(
                t.text,
                SourceLocation{t.line, t.column}
            );
        }

        case TokenType::BoolLiteral: {
            return std::make_unique<BoolExpr>(
                t.text == "true",
                SourceLocation{t.line, t.column}
            );
        }

        case TokenType::Identifier: {
            if (peek().type == TokenType::LParen) {
                advance();

                std::vector<std::unique_ptr<Expr>> args;

                if (peek().type != TokenType::RParen) {
                    do {
                        args.push_back(parseExpression(Precedence::Assignment));
                    } while (match(TokenType::Comma));
                }

                if (!match(TokenType::RParen)) {
                    throw CompileError("Expected ')'", {t.line, t.column});
                }

                return std::make_unique<CallExpr>(
                    t.text,
                    std::move(args),
                    SourceLocation{t.line, t.column}
                );
            }

            return std::make_unique<VariableExpr>(
                t.text,
                SourceLocation{t.line, t.column}
            );
        }

        case TokenType::LParen: {
            auto expr = parseExpression(Precedence::Assignment);

            if (!match(TokenType::RParen)) {
                throw CompileError("Expected ')'", {t.line, t.column});
            }

            return expr;
        }

        case TokenType::LBracket: {
            return parseList();
        }

        default:
            throw CompileError("Unexpected token: " + t.text, {t.line, t.column});
    }
}

std::unique_ptr<Expr> Parser::parseInfix(std::unique_ptr<Expr> left, const Token& op) {
    Precedence prec = getPrecedence(op.type);

    if (op.type == TokenType::DDot) {
        Token name = advance();

        if (name.type != TokenType::Identifier) {
            throw CompileError("Expected property name after :", {name.line, name.column});
        }

        return std::make_unique<GetExpr>(
            std::move(left),
            name.text,
            SourceLocation{op.line, op.column}
        );
    }

    if (op.type == TokenType::LBracket) {
        auto index = parseExpression(Precedence::None);

        if (!match(TokenType::RBracket)) {
            throw RuntimeError("Expected ']'", {op.line, op.column});
        }

        return std::make_unique<IndexExpr>(
            std::move(left),
            std::move(index),
            SourceLocation{op.line, op.column}
        );
    }

    if (op.type == TokenType::Dot) {
        Token name = advance();

        if (name.type != TokenType::Identifier) {
            throw CompileError("Expected method name after .", {name.line, name.column});
        }

        if (!match(TokenType::LParen)) {
            throw RuntimeError("Expected '(' after method", {peek().line, peek().column});
        }

        std::vector<std::unique_ptr<Expr>> args;

        if (peek().type != TokenType::RParen) {
            do {
                args.push_back(parseExpression(Precedence::Assignment));
            } while (match(TokenType::Comma));
        }

        if (!match(TokenType::RParen)) {
            throw CompileError("Expected ')'", {peek().line, peek().column});
        }

        return std::make_unique<MemberCallExpr>(
            std::move(left),
            name.text,
            std::move(args),
            SourceLocation{name.line, name.column}
        );
    }

    bool rightAssoc = (op.type == TokenType::Power);

    auto right = parseExpression(
        rightAssoc ? static_cast<Precedence>(static_cast<int>(prec) - 1) : prec
    );

    return std::make_unique<BinaryExpr>(
        tokenToOp(op.type),
        std::move(left),
        std::move(right),
        SourceLocation{op.line, op.column}
    );
}

std::unique_ptr<Expr> Parser::parseExpression(Precedence prec) {
    Token t = advance();

    auto left = parsePrefix(t);

    while (getPrecedence(peek().type) > prec) {
        Token op = advance();
        left = parseInfix(std::move(left), op);
    }

    return left;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (match(TokenType::Var)) {
        Token name = advance();

        if (!match(TokenType::Equal))
            throw CompileError("Expected '='", {name.line, name.column});

        ValueType vtype = ValueType::Undefined;

        if (match(TokenType::LParen)) {

            Token tokenType = advance();

            if (tokenType.type == TokenType::Identifier) {
                if (tokenType.text == "int")
                    vtype = ValueType::Int;
                else if (tokenType.text == "float")
                    vtype = ValueType::Float;
                else if (tokenType.text == "str" || tokenType.text == "string")
                    vtype = ValueType::String;
                else if (tokenType.text == "bool")
                    vtype = ValueType::Bool;
                else if (tokenType.text == "list") {
                    vtype = ValueType::List;
                }
                else
                    throw ValueError("Expected type, got " + tokenType.text, {tokenType.line, tokenType.column});
            }

            if (!match(TokenType::RParen))
                throw CompileError("Expected ')'", {tokenType.line, tokenType.column});
        }

        auto value = parseExpression(Precedence::Assignment);

        match(TokenType::Newline);

        return std::make_unique<VarStmt>(
            name.text,
            vtype,
            std::move(value),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (peek().type == TokenType::Identifier && pos+1 < tokens.size() && tokens[pos+1].type == TokenType::Equal) {
        Token name = advance();
        advance();

        auto value = parseExpression(Precedence::Assignment);

        match(TokenType::Newline);

        return std::make_unique<AssignStmt>(
            name.text,
            std::move(value),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::Del)) {

        auto names = parseIdentifierList();

        match(TokenType::Newline);

        return std::make_unique<DeleteStmt>(
            names,
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::If)) {
        if (!match(TokenType::LParen)) {
            throw CompileError("Expected '('", {peek().line, peek().column});
        }

        auto condition = parseExpression(Precedence::Assignment);

        if (!match(TokenType::RParen)) {
            throw CompileError("Expected ')'", {peek().line, peek().column});
        }
        if (!match(TokenType::Do)) {
            throw RuntimeError("Expected 'do' after if", {peek().line, peek().column});
        }

        std::vector<std::unique_ptr<Stmt>> thenBranch;
        while (peek().type != TokenType::Done && peek().type != TokenType::Else) {
            thenBranch.push_back(parseStatement());
        }

        std::vector<std::unique_ptr<Stmt>> elseBranch;
        if (match(TokenType::Else)) {
            if (!match(TokenType::Do)) {
                throw RuntimeError("Expected 'do' after else", {peek().line, peek().column});
            }
            while (peek().type != TokenType::Done) {
                elseBranch.push_back(parseStatement());
            }
        }

        if (!match(TokenType::Done))
            throw RuntimeError("Expected 'done'", {peek().line, peek().column});

        return std::make_unique<IfStmt>(
            std::move(condition),
            std::move(thenBranch),
            std::move(elseBranch),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::While)) {
        if (!match(TokenType::LParen)) {
            throw CompileError("Expected '('", {peek().line, peek().column});
        }

        auto condition = parseExpression(Precedence::Assignment);

        if (!match(TokenType::RParen)) {
            throw CompileError("Expected ')'", {peek().line, peek().column});
        }

        if (!match(TokenType::Do)) {
            throw CompileError("Expected 'do' after while", {peek().line, peek().column});
        }
        std::vector<std::unique_ptr<Stmt>> content;

        while (peek().type != TokenType::Done && peek().type != TokenType::End) {
            content.push_back(parseStatement());
        }

        if (!match(TokenType::Done)) {
            throw CompileError("Expected 'done'", {peek().line, peek().column});
        }

        return std::make_unique<WhileStmt>(
            std::move(condition),
            std::move(content),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::For)) {
        if (!match(TokenType::LParen)) {
            throw CompileError("Expected '('", {peek().line, peek().column});
        }

        Token varName = advance();

        if (varName.type != TokenType::Identifier) {
            throw CompileError("Expected identifier", {peek().line, peek().column});
        }

        if (!match(TokenType::In)) {
            throw CompileError("Expected 'in' after for", {peek().line, peek().column});
        }

        auto iterable = parseExpression(Precedence::None);

        if (!match(TokenType::RParen)) {
            throw CompileError("Expected ')'", {peek().line, peek().column});
        }

        if (!match(TokenType::Do)) {
            throw CompileError("Expected 'do' after for", {peek().line, peek().column});
        }

        std::vector<std::unique_ptr<Stmt>> content;

        while (peek().type != TokenType::Done && peek().type != TokenType::End) {
            content.push_back(parseStatement());
        }

        if (!match(TokenType::Done)) {
            throw CompileError("Expected 'done'", {peek().line, peek().column});
        }

        return std::make_unique<ForStmt>(
            varName.text,
            std::move(iterable),
            std::move(content),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::Fun)) {
        ValueType vtype = ValueType::Undefined;

        if (match(TokenType::LParen)) {
            Token tokenType = advance();
            if (tokenType.type == TokenType::Identifier) {
                if (tokenType.text == "int")
                    vtype = ValueType::Int;
                else if (tokenType.text == "float")
                    vtype = ValueType::Float;
                else if (tokenType.text == "str" || tokenType.text == "string")
                    vtype = ValueType::String;
                else if (tokenType.text == "bool")
                    vtype = ValueType::Bool;
                else
                    throw ValueError("Expected type, got " + tokenType.text, {tokenType.line, tokenType.column});
            }

            if (!match(TokenType::RParen)) throw RuntimeError("Expected ')'", {tokenType.line, tokenType.column});
        }

        if (peek().type == TokenType::End) throw CompileError("Expected name of the function", {peek().line, peek().column});

        Token name = advance();

        if (!match(TokenType::LParen)) throw RuntimeError("Expected '(' after definition", {peek().line, peek().column});

        std::vector<std::string> params;

        if (peek().type != TokenType::RParen)
            params = parseIdentifierList();

        if (!match(TokenType::RParen)) throw RuntimeError("Expected ')' after definition", {peek().line, peek().column});

        if (!match(TokenType::Do)) throw RuntimeError("Expected 'do' after definition", {peek().line, peek().column});

        std::vector<std::unique_ptr<Stmt>> body;

        while (peek().type != TokenType::Done && peek().type != TokenType::End) {
            body.push_back(parseStatement());
        }

        if (!match(TokenType::Done)) throw RuntimeError("Expected 'done' after 'do'", {peek().line, peek().column});

        return std::make_unique<FunStmt>(
            std::move(name.text),
            std::move(vtype),
            std::move(params),
            std::move(body),
            SourceLocation{peek().line, peek().column}
        );
    }

    if (match(TokenType::Import)) {

        std::vector<std::string> names;

        do {

            Token name = advance();

            if (name.type != TokenType::Identifier && name.type != TokenType::StringLiteral)
                throw RuntimeError("Expected module name, got something else", {peek().line, peek().column});

            names.push_back(name.text);

        } while (match(TokenType::Comma));

        match(TokenType::Newline);

        return std::make_unique<ImportStmt>(
            names,
            SourceLocation{peek().line, peek().column}
        );
    }

    std::vector<std::unique_ptr<Expr>> expr;

    expr = parseExpressionList(Precedence::Assignment);

    match(TokenType::Newline);

    return std::make_unique<ExpressionStmt>(
        std::move(expr),
        SourceLocation{peek().line, peek().column}
    );
}

std::unique_ptr<Expr> Parser::parseList() {
    std::vector<std::unique_ptr<Expr>> elements = parseExpressionList(Precedence::None);

    if (!match(TokenType::RBracket)) {
        throw CompileError("Expected ']'", {peek().line, peek().column});
    }

    return std::make_unique<ListExpr>(
        std::move(elements),
        SourceLocation{peek().line, peek().column}
    );
}

std::vector<std::string> Parser::parseIdentifierList() {
    std::vector<std::string> names;

    do {

        Token name = advance();

        if (name.type != TokenType::Identifier)
            throw RuntimeError("Expected variable name, got something else", {peek().line, peek().column});

        names.push_back(name.text);

    } while (match(TokenType::Comma));

    return names;
}

std::vector<std::unique_ptr<Expr>> Parser::parseExpressionList(Precedence prec) {
    std::vector<std::unique_ptr<Expr>> values;

    if (peek().type == TokenType::RParen || peek().type == TokenType::RBracket) return values;

    do {
        values.push_back(parseExpression(prec));
    } while (match(TokenType::Comma));

    return values;
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> program;

    while (peek().type != TokenType::End) {
        program.push_back(parseStatement());
    }

    return program;
}
