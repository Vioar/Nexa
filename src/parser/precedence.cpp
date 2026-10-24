// precedence.cpp

#include "parser/precedence.hpp"

Precedence getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::EqualEqual:
        case TokenType::NotEqual:
            return Precedence::Equality;

        case TokenType::Equal:
            return Precedence::Assignment;

        case TokenType::Less:
        case TokenType::Greater:
        case TokenType::LessEqual:
        case TokenType::GreaterEqual:
            return Precedence::Comparison;

        case TokenType::Plus:
        case TokenType::Minus:
            return Precedence::Term;

        case TokenType::Star:
        case TokenType::Slash:
            return Precedence::Factor;

        case TokenType::Power:
            return Precedence::Power;

        case TokenType::LParen:
        case TokenType::Dot:
        case TokenType::DDot:
        case TokenType::LBracket:
            return Precedence::Call;

        default:
            return Precedence::None;
    }
}
