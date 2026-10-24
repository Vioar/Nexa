// methods.hpp

#pragma once

#include <functional>
#include <string>

#include "runtime/value.hpp"
#include "ast/ast.hpp"
#include "runtime/execution_context.hpp"

using moduleFunc = std::function<Value(ExecutionContext&, Value, const std::vector<std::unique_ptr<Expr>>&, SourceLocation&)>;

Value copyMethod(ExecutionContext& exec_cont, Value object, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

inline std::unordered_map<std::string, moduleFunc> allMethods = {
    {"copy", copyMethod}
};
