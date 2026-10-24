// environment.hpp

#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "runtime/value.hpp"
#include "ast/ast.hpp"

struct Variable {
    ValueType type;
    Value value;
};

struct Function {
    ValueType return_type;
    std::vector<std::string> args;
    std::shared_ptr<std::vector<std::unique_ptr<Stmt>>> body;
};

struct Function_to_execute {
    ValueType return_type;
    std::vector<std::string> args;
    std::shared_ptr<std::vector<std::unique_ptr<Stmt>>> body;
};

class Environment {
public:
    bool replMode = false;
    std::unordered_map<std::string, Variable> vars;
    std::unordered_map<std::string, Function> funcs;

    Environment* parent;

    Environment(
        Environment* parent = nullptr
    );

    void declare(const std::string& name,
                 ValueType type,
                 Value value,
                 SourceLocation loc = {0, 0});

    void set(const std::string& name, Value value, SourceLocation loc = {0, 0});

    Value get(const std::string& name, SourceLocation loc = {0, 0});

    bool exists(const std::string& name);
    bool exists_local(const std::string& name);

    void remove(const std::string& name, SourceLocation loc = {0, 0});

    bool exists_func(const std::string& name);
    bool exists_func_local(const std::string& name);

    void new_func(const std::string& name,
        ValueType type,
        std::vector<std::string> args,
        std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body,
        SourceLocation loc = {0, 0}
    );

    const Function_to_execute get_func(const std::string& name, SourceLocation loc = {0, 0});
};
