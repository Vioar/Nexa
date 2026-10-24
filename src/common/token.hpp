// token.hpp

#pragma once

#include <string>

enum class TokenType {
    End, // The end

    Number,
    StringLiteral,
    BoolLiteral,
    Identifier,

    Var, // var
    Del, // del
    Int, // int
    Float, // float
    String, //string
    Bool, // bool

    Print, // print

    If, // if
    Elif, // elif
    Else, // else
    Not, // !, not
    While, // while
    For, // for
    Fun, // fun

    In, // in

    Do, // do
    Done, // done

    Plus, // +
    Minus, // -
    Star, // *
    Slash, // /
    Power, // ^

    Equal, // =
    EqualEqual, // ==
    NotEqual, // !=

    Greater, // >
    Less, // <
    GreaterEqual, // >=
    LessEqual, // <=

    LParen, // (
    RParen, // )
    LBrace, // {
    RBrace, // }
    LBracket, // [
    RBracket, // ]

    Semicolon, // ;
    Newline,  // \n
    DoubleQuotation,  // "
    SingleQuotation,  // '
    Comma,  // ,
    Dot, // .
    DDot, // :

    Import  // import
};

struct Token {
    TokenType type;
    std::string text;

    int line;
    int column;
};
