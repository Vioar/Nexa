// precedence.hpp

#pragma once

#include "common/token.hpp"

enum class Precedence {
    None = 0,
    Assignment,  // =
    Equality,  // == !=
    Comparison,  // < > <= >=
    Term,  // + -
    Factor,  // * /
    Prefix,  // -x !x
    Power,  // ^
    Call  // ()
};

Precedence getPrecedence(TokenType type);
