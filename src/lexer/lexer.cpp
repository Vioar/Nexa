// lexer.cpp

#include <cctype>

#include "common/error.hpp"
#include "lexer/lexer.hpp"

Lexer::Lexer(const std::string& text) : src(text) {}

char Lexer::peek() {
    if (pos >= src.size()) return '\0';
    return src[pos];
}

char Lexer::peekNext() {
    if (pos + 1 >= src.size()) return '\0';
    return src[pos + 1];
}

char Lexer::advance() {
    char c = src[pos++];

    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::startsWith(const std::string& s) {
    return src.substr(pos, s.size()) == s;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (true) {
        skipWhitespace();
        char c = peek();

        int startLine = line;
        int startColumn = column;

        if (c == '-' && peekNext() == '/') {
            advance();
            advance();

            while (peek() != '\0' && !(peek() == '/' && peekNext() == '-')) advance();

            if (peek() == '\0') {
                throw CompileError("Expected '/-' to end multiline comment", {line, column});
            }

            if (peek() == '/' && peekNext() == '-') {
                advance();
                advance();
            }

            continue;
        }

        if (c == '-' && peekNext() == '-') {
            advance();
            advance();

            while (peek() != '\n' && peek() != '\0') advance();

            continue;
        }

        if (c == '\0') {
            tokens.push_back({TokenType::End, "", startLine, startColumn});
            break;
        }

        if (isdigit(c)) {
            std::string num;
            bool hasDot = false;

            while (isdigit(peek()) || peek() == '.') {
                if (peek() == '.') {
                    if (hasDot)
                        throw CompileError("Unexpected char: '.'", {line, column});

                    hasDot = true;
                }
                num += advance();
            }

            tokens.push_back({TokenType::Number, num, startLine, startColumn});
            continue;
        }

        if (isalpha(c) || c == '_') {
            std::string word;
            while (isalnum(peek()) || peek() == '_')
                word += advance();

            TokenType type = TokenType::Identifier;

            if (word == "var") type = TokenType::Var;
            else if (word == "del") type = TokenType::Del;

            else if (word == "true" || word == "false") type = TokenType::BoolLiteral;

            else if (word == "if") type = TokenType::If;
            else if (word == "else") type = TokenType::Else;
            else if (word == "elif") type = TokenType::Elif;
            else if (word == "do") type = TokenType::Do;
            else if (word == "done") type = TokenType::Done;
            else if (word == "fun") type = TokenType::Fun;

            else if (word == "while") type = TokenType::While;
            else if (word == "for") type = TokenType::For;

            else if (word == "in") type = TokenType::In;

            else if (word == "import") type = TokenType::Import;

            tokens.push_back({type, word, startLine, startColumn});

            continue;
        }

        if (c == '"') {
            advance();

            std::string text;

            while (peek() != '"' && peek() != '\0') {
                if (peek() == '\n') {
                    line++;
                    column = 1;
                }

                if (peek() == '\\') {
                    advance();

                    char esc = advance();

                    switch (esc) {
                        case 'n': text += '\n'; break;
                        case 't': text += '\t'; break;
                        case '"': text += '"'; break;
                        case '\\': text += '\\'; break;

                        default: {
                            text += '\\';
                            text += esc;
                            break;
                        }
                    }
                } else
                    text += advance();
            }

            if (peek() != '"')
                throw CompileError("Expected '\"'", {line, column});

            advance();

            tokens.push_back({TokenType::StringLiteral, text, startLine, startColumn});

            continue;
        }

        switch (advance()) {
            case '+': tokens.push_back({TokenType::Plus, "+", startLine, startColumn}); break;
            case '-': tokens.push_back({TokenType::Minus, "-", startLine, startColumn}); break;
            case '*': tokens.push_back({TokenType::Star, "*", startLine, startColumn}); break;
            case '/': tokens.push_back({TokenType::Slash, "/", startLine, startColumn}); break;
            case '^': tokens.push_back({TokenType::Power, "^", startLine, startColumn}); break;
            case '=': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back({TokenType::EqualEqual, "==", startLine, startColumn});
                    break;
                }
                tokens.push_back({TokenType::Equal, "=", startLine, startColumn});
                break;
            }

            case '<': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back({TokenType::LessEqual, "<=", startLine, startColumn});
                    break;
                }
                tokens.push_back({TokenType::Less, "<", startLine, startColumn});
                break;
            }
            case '>': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back({TokenType::GreaterEqual, ">=", startLine, startColumn});
                    break;
                }
                tokens.push_back({TokenType::Greater, ">", startLine, startColumn});
                break;
            }
            case '!': {
                if (peek() == '=') {
                    advance();
                    tokens.push_back({TokenType::NotEqual, "!=", startLine, startColumn});
                    break;
                }
                tokens.push_back({TokenType::Not, "!", startLine, startColumn});
                break;
            }

            case '(': tokens.push_back({TokenType::LParen, "(", startLine, startColumn}); break;
            case ')': tokens.push_back({TokenType::RParen, ")", startLine, startColumn}); break;
            case '{': tokens.push_back({TokenType::LBrace, "{", startLine, startColumn}); break;
            case '}': tokens.push_back({TokenType::RBrace, "}", startLine, startColumn}); break;
            case '[': tokens.push_back({TokenType::LBracket, "[", startLine, startColumn}); break;
            case ']': tokens.push_back({TokenType::RBracket, "]", startLine, startColumn}); break;

            case ';': tokens.push_back({TokenType::Semicolon, ";", startLine, startColumn}); break;
            case '\n': tokens.push_back({TokenType::Newline, "\n", startLine, startColumn}); break;
            case ',': tokens.push_back({TokenType::Comma, ",", startLine, startColumn}); break;
            case ':': tokens.push_back({TokenType::DDot, ":", startLine, startColumn}); break;
            case '.': tokens.push_back({TokenType::Dot, ".", startLine, startColumn}); break;

            default:
                throw CompileError(std::string("Unexpected char: ") + c, {line, column});
        }
    }

    return tokens;
}
