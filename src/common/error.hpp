// error.hpp

#pragma once

#include <stdexcept>
#include <string>

struct SourceLocation {
    int line;
    int column;
};

class RuntimeError : public std::runtime_error {
public:
    SourceLocation loc;

    RuntimeError(const std::string& msg, SourceLocation l)
        : std::runtime_error(msg), loc(l) {}
};

class CompileError : public std::runtime_error {
public:
    SourceLocation loc;

    CompileError(const std::string& msg, SourceLocation l)
        : std::runtime_error(msg), loc(l) {}
};

class ValueError : public std::runtime_error {
public:
    SourceLocation loc;

    ValueError(const std::string& msg, SourceLocation l)
        : std::runtime_error(msg), loc(l) {}
};

class TypeError : public std::runtime_error {
public:
    SourceLocation loc;

    TypeError(const std::string& msg, SourceLocation l)
        : std::runtime_error(msg), loc(l) {}
};

class MathError : public std::runtime_error {
public:
    SourceLocation loc;

    MathError(const std::string& msg, SourceLocation l)
        : std::runtime_error(msg), loc(l) {}
};
