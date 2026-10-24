// natives.cpp

#include <error.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "builtins/natives.hpp"
#include "common/error.hpp"
#include "runtime/operations.hpp"
#include "common/types.hpp"
#include "runtime/value.hpp"

Value printBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    for (size_t i = 0; i < args.size(); i++) {
        Value v = args[i]->eval(exec_cont);
        std::cout << toString(v, loc);

        if (i+1 < args.size()) {
            std::cout << ", ";
        }
    }
    std::cout<<std::endl;

    return {ValueType::Undefined, std::monostate{}};
}

Value toStrBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    if (args.size() != 1) {
            throw RuntimeError("String conversion expects 1 argument", loc);
        }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::String, toString(v, loc)};
}

Value toIntBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    if (args.size() != 1) {
        throw RuntimeError("int() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::Int, toInt(v, loc)};
}

Value toFloatBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    if (args.size() != 1) {
        throw RuntimeError("int() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::Float, toFloat(v, loc)};
}

Value toBoolBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    if (args.size() != 1) {
        throw RuntimeError("bool() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::Bool, toBool(v, loc)};
}

Value toListBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("list() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::List, toList(v, loc)};
}

Value getTypeBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>>& args, SourceLocation& loc) {
    if (args.size() != 1) {
        throw RuntimeError("type() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    return {ValueType::String, typeToStr(v.type)};
}

Value getLengthBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("len() expects 1 argument", loc);
    }

    Value v = args[0]->eval(exec_cont);

    switch (v.type) {
        case ValueType::String: return {ValueType::Int, static_cast<int>(toString(v, loc).length())};
        case ValueType::List: return {ValueType::Int, static_cast<int>(std::get<List>(v.data)->size())};

        case ValueType::Int:
        case ValueType::Float:
        case ValueType::Bool:
        case ValueType::Undefined:
            throw RuntimeError(typeToStr(v.type) + "'s length can't be mesured", loc);
    }

    throw RuntimeError(typeToStr(v.type) + "'s length can't be mesured", loc);
}

Value rangeBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {

    Value start;
    Value end;
    Value step;

    switch (args.size()) {
        case 1: {
            start = {ValueType::Int, 0};
            end = args[0]->eval(exec_cont);
            step = {ValueType::Int, 1};
            break;
        }
        case 2: {
            start = args[0]->eval(exec_cont);
            end = args[1]->eval(exec_cont);
            step = {ValueType::Int, 1};
            break;
        }
        case 3: {
            start = args[0]->eval(exec_cont);
            end = args[1]->eval(exec_cont);
            step = args[2]->eval(exec_cont);
            break;
        }
        default: throw RuntimeError("range() expects at least 1 argument, got " + std::to_string(args.size()), loc);
    }

    for (auto& i : {start, end, step}) {
        if (i.type != ValueType::Int) {
            throw RuntimeError("range() takes only int as arguments", loc);
        }
    }

    List out = std::make_shared<std::vector<Value>>();

    for (int i = std::get<int>(start.data); i < std::get<int>(end.data); i += std::get<int>(step.data)) {
        out->push_back(Value{ValueType::Int, i});
    }

    return {ValueType::List, out};
}

Value inputBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() > 1) {
        throw RuntimeError("input() expects at most 1 argument, got " + std::to_string(args.size()), loc);
    }

    std::string prompt = "";
    std::string out;

    if (args.size() == 1) {
        Value v = args[0]->eval(exec_cont);
        prompt = toString(v, loc);
    }

    std::cout << prompt;

    std::getline(std::cin, out);

    return {ValueType::String, out};
}

Value minBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() < 2) {
        throw RuntimeError("min() expects at least 2 arguments, got " + std::to_string(args.size()), loc);
    }

    for (auto& i : args) {
        ValueType type = i->eval(exec_cont).type;
        if (!Operations::isNumber(type)) {
            throw RuntimeError("min() expected a number, got " + typeToStr(type), loc);
        }
    }

    Value min_val = args[0]->eval(exec_cont);
    for (auto& i : args) {
        Value v = i->eval(exec_cont);
        if (v.asFloat() < min_val.asFloat()) {
            min_val = v;
        }
    }

    return min_val;
}

Value maxBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() < 2) {
        throw RuntimeError("min() expects at least 2 arguments, got " + std::to_string(args.size()), loc);
    }

    for (auto& i : args) {
        ValueType type = i->eval(exec_cont).type;
        if (!Operations::isNumber(type)) {
            throw RuntimeError("min() expected a number, got " + typeToStr(type), loc);
        }
    }

    Value max_val = args[0]->eval(exec_cont);
    for (auto& i : args) {
        Value v = i->eval(exec_cont);
        if (v.asFloat() > max_val.asFloat()) {
            max_val = v;
        }
    }

    return max_val;
}

Value absBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("abs() expects 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);

    if (!Operations::isNumber(v.type)) {
        throw RuntimeError("abs() expected a number, got " + typeToStr(v.type), loc);
    }

    switch (v.type) {
        case ValueType::Int: {
            if (std::get<int>(v.data) < 0) {
                return {ValueType::Int, -std::get<int>(v.data)};
            } else {
                return v;
            }
        }

        case ValueType::Float: {
            if (std::get<double>(v.data) < 0.0) {
                return {ValueType::Float, -std::get<double>(v.data)};
            } else {
                return v;
            }
        }

        case ValueType::Bool:
        case ValueType::String:
        case ValueType::List:
        case ValueType::Undefined:
        default:
            throw RuntimeError("abs() expected a number, got " + typeToStr(v.type), loc);
    }
}

Value sumListsBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("sum() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);
    if (v.type != ValueType::List) {
        throw RuntimeError("sum() expected list, got " + typeToStr(v.type), loc);
    }

    List list = std::get<List>(v.data);
    for (auto& i : *list) {
        if (!Operations::isNumber(i.type)) {
            throw RuntimeError("All elements of list must be numbers", loc);
        }
    }

    double out = 0;

    for (auto& i : *list) {
        if (i.type == ValueType::Float) {
            out += std::get<double>(i.data);
        } else if (i.type == ValueType::Int) {
            out += std::get<int>(i.data);
        } else if (i.type == ValueType::Bool) {
            out += std::get<bool>(i.data);
        } else {
            throw RuntimeError("All elements of list must be numbers", loc);
        }
    }

    return {ValueType::Float, out};
}

Value anyListTrueBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("any() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);
    if (v.type != ValueType::List) {
        throw RuntimeError("any() expected list, got " + typeToStr(v.type), loc);
    }

    List list = std::get<List>(v.data);

    for (auto& i : *list) {
        if (i.isTruthy()) {
            return {ValueType::Bool, true};
        }
    }

    return {ValueType::Bool, false};
}

Value allListTrueBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("any() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);
    if (v.type != ValueType::List) {
        throw RuntimeError("any() expected list, got " + typeToStr(v.type), loc);
    }

    List list = std::get<List>(v.data);

    for (auto& i : *list) {
        if (!i.isTruthy()) {
            return {ValueType::Bool, false};
        }
    }

    return {ValueType::Bool, true};
}


Value roundBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("round() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);

    if (v.type != ValueType::Float) {
        throw RuntimeError("round() expected float, got " + typeToStr(v.type), loc);
    }
    double i = std::get<double>(v.data);

    return {ValueType::Int, i-static_cast<int>(i) >= 0.5 ? static_cast<int>(i) + 1 : static_cast<int>(i)};
}

Value floorBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("round() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);

    if (v.type != ValueType::Float) {
        throw RuntimeError("round() expected float, got " + typeToStr(v.type), loc);
    }
    double i = std::get<double>(v.data);

    if (static_cast<double>(static_cast<int>(i)) == i) {
        return {ValueType::Int, static_cast<int>(i)};
    }
    if (i > 0.0) {
       return {ValueType::Int, static_cast<int>(i)};
    } else if (i < 0.0) {
        return {ValueType::Int, static_cast<int>(i)-1};
    }

    return {ValueType::Int, static_cast<int>(i)};
}

Value ceilBuiltinFun(ExecutionContext& exec_cont, const std::vector<std::unique_ptr<Expr>> &args, SourceLocation loc) {
    if (args.size() != 1) {
        throw RuntimeError("round() expected 1 argument, got " + std::to_string(args.size()), loc);
    }

    Value v = args[0]->eval(exec_cont);

    if (v.type != ValueType::Float) {
        throw RuntimeError("round() expected float, got " + typeToStr(v.type), loc);
    }
    double i = std::get<double>(v.data);

    if (static_cast<double>(static_cast<int>(i)) == i) {
        return {ValueType::Int, static_cast<int>(i)};
    }
    if (i > 0.0) {
       return {ValueType::Int, static_cast<int>(i)+1};
    } else if (i < 0.0) {
        return {ValueType::Int, static_cast<int>(i)};
    }

    return {ValueType::Int, static_cast<int>(i)};
}
