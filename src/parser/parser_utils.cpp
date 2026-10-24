// parser_utils.cpp

#include <stdexcept>

#include "parser/parser_utils.hpp"

Op tokenToOp(TokenType t) {
    switch (t) {
        case TokenType::Plus: return Op::Add;
        case TokenType::Minus: return Op::Sub;
        case TokenType::Slash: return Op::Div;
        case TokenType::Star: return Op::Mul;
        case TokenType::Power: return Op::Pow;

        case TokenType::Less: return Op::Less;
        case TokenType::Greater: return Op::Greater;
        case TokenType::LessEqual: return Op::LessEqual;
        case TokenType::GreaterEqual: return Op::GreaterEqual;

        case TokenType::EqualEqual: return Op::Equal;
        case TokenType::NotEqual: return Op::NotEqual;

        default: throw std::runtime_error("Invalid operator token");
    }
}
