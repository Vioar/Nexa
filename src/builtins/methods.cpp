// methods.cpp

#include "methods.hpp"
#include "runtime/execution_context.hpp"

Value copyMethod(ExecutionContext& exec_cont, Value object, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation loc) {
    if (args.size() > 1) {
        throw RuntimeError("copy() takes 1 argument, got " + std::to_string(args.size()), loc);
    }

    bool deep = false;

    if (args.size() == 1) {
        Value arg_1 = args[0]->eval(exec_cont);
        if (arg_1.type != ValueType::Bool) {
            throw RuntimeError("copy() expected bool, got " + typeToStr(arg_1.type), loc);
        } else {
            deep = std::get<bool>(arg_1.data);
        }
    }

    return copy(object, deep, loc);
}
