// natives.hpp

#pragma once

#include <error.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

#include "common/error.hpp"
#include "runtime/value.hpp"
#include "ast/ast.hpp"
#include "runtime/execution_context.hpp"

using NativeFunc = std::function<Value(ExecutionContext&, const std::vector<std::unique_ptr<Expr>>&, SourceLocation&)>;

Value printBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value inputBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value toStrBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value toIntBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value toFloatBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value toBoolBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value toListBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value getTypeBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc);
Value getLengthBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value rangeBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value sumListsBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value anyListTrueBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value allListTrueBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value minBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value maxBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value absBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

Value roundBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value floorBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);
Value ceilBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc);

inline std::unordered_map<std::string, NativeFunc> natives = {
    {"print", printBuiltinFun},
    {"str", toStrBuiltinFun},
    {"string", toStrBuiltinFun},
    {"int", toIntBuiltinFun},
    {"float", toFloatBuiltinFun},
    {"bool", toBoolBuiltinFun},
    {"list", toListBuiltinFun},
    {"type", getTypeBuiltinFun},
    {"len", getLengthBuiltinFun},
    {"range", rangeBuiltinFun},
    {"input", inputBuiltinFun},
    {"min", minBuiltinFun},
    {"max", maxBuiltinFun},
    {"abs", absBuiltinFun},
    {"sum", sumListsBuiltinFun},
    {"any", anyListTrueBuiltinFun},
    {"all", allListTrueBuiltinFun},
    {"round", roundBuiltinFun},
    {"floor", floorBuiltinFun},
    {"ceil", ceilBuiltinFun}
};
