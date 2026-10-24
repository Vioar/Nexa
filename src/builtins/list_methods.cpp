// list_methods.cpp

#include "builtins/list_methods.hpp"
#include "common/error.hpp"
#include "common/types.hpp"
#include "runtime/execution_context.hpp"
#include "runtime/value.hpp"
#include <string>

Value listAddListMethod(ExecutionContext &exec_cont, Value object, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("add() takes 1 argument, got " + std::to_string(args.size()), loc);
    }

    auto& list = std::get<List>(object.data);

    list->push_back(args[0]->eval(exec_cont));

    return {ValueType::Undefined, std::monostate()};
}

Value listPopListMethod(ExecutionContext &exec_cont, Value object, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() > 1) {
        throw RuntimeError("pop() takes at most 1 argument, got " + std::to_string(args.size()), loc);
    }

    auto& list = std::get<List>(object.data);

    if (list->empty()) {
        throw RuntimeError("pop() from empty list", loc);
    }

    int index = int(list->size())-1;

    if (index == -1) {
        index = int(list->size())-1;
    }

    if (args.size() == 1) {
        Value arg_1 = args[0]->eval(exec_cont);
        if (arg_1.type != ValueType::Int) {
            throw RuntimeError("pop() expected int, got " + typeToStr(arg_1.type), loc);
        } else {
            index = std::get<int>(arg_1.data);
        }
    }

    if (index < 0 || index >= static_cast<int>(list->size())) {
        throw RuntimeError("Index out of range", loc);
    }

    list->erase(list->begin() + index);

    return {ValueType::Undefined, std::monostate()};
}

/*
Value listCopyMethod(Environment& env, Value object, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() > 1) {
        throw RuntimeError("copy() takes at most 1 argument, got " + std::to_string(args.size()), loc);
    }

    bool deep = false;

    if (args.size() == 1) {
        Value arg_1 = args[0]->eval(env);
        if (arg_1.type != ValueType::Bool) {
            throw RuntimeError("copy() expected bool, got " + typeToStr(arg_1.type), loc);
        } else {
            deep = std::get<bool>(arg_1.data);
        }
    }

    return copy(object, deep, loc);
}
*/
