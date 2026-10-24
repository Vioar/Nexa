// list_methods.hpp

#pragma once

#include <error.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "runtime/value.hpp"
#include "ast/ast.hpp"
#include "runtime/execution_context.hpp"

using ListModuleFunc = std::function<Value(ExecutionContext&, Value, const std::vector<std::unique_ptr<Expr>>&, SourceLocation&)>;

Value listAddListMethod(ExecutionContext& exec_cont, Value object, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value listPopListMethod(ExecutionContext& exec_cont, Value object, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

inline std::unordered_map<std::string, ListModuleFunc> listMethods = {
    {"add", listAddListMethod},
    {"pop", listPopListMethod}
};
