// ast.cpp

#include "runtime/execution_context.hpp"
#include <error.h>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>
namespace fs = std::filesystem;

#include "ast/ast.hpp"
#include "builtins/methods.hpp"
#include "runtime/execution_context.hpp"
#include "builtins/natives.hpp"
#include "runtime/operations.hpp"
#include "runtime/environment.hpp"
#include "runtime/runtime.hpp"
#include "module/module.hpp"
#include "common/types.hpp"
#include "runtime/value.hpp"
#include "builtins/list_methods.hpp"

Value NumberExpr::eval(ExecutionContext&) {
    return {ValueType::Int, value};
}

Value FloatExpr::eval(ExecutionContext&) {
    return {ValueType::Float, value};
}

Value UnaryOp::eval(ExecutionContext& exec_con) {
    Value v = operand->eval(exec_con);

    switch (op) {
        case Op::Sub: {
            switch (v.type) {
                case ValueType::Int:
                    return {ValueType::Int, -std::get<int>(v.data)};
                case ValueType::Float:
                    return {ValueType::Float, -std::get<double>(v.data)};

                case ValueType::Bool:
                case ValueType::List:
                case ValueType::Undefined:
                default:
                    throw RuntimeError("Unexpected syntax", loc);
            }
        }
        case Op::Add: {
            switch (v.type) {
                case ValueType::Int:
                    return {ValueType::Int, std::get<int>(v.data)};
                case ValueType::Float:
                    return {ValueType::Float, std::get<double>(v.data)};

                case ValueType::Bool:
                case ValueType::List:
                case ValueType::Undefined:
                default:
                    throw RuntimeError("Unexpected syntax", loc);
            }
        }

        case Op::Div:
        case Op::Pow:
        case Op::Less:
        case Op::Greater:
        case Op::LessEqual:
        case Op::GreaterEqual:
        case Op::Equal:
        case Op::NotEqual:
        default:
            throw RuntimeError("Unexpected syntax", loc);
    }
}

Value StringExpr::eval(ExecutionContext&) {
    return {ValueType::String, value};
}

Value BoolExpr::eval(ExecutionContext&) {
    return {ValueType::Bool, value};
}

Value ListExpr::eval(ExecutionContext& exec_con) {
    List result = std::make_shared<std::vector<Value>>();

    result->reserve(elements.size());

    for (auto& e : elements) {
        result->push_back(Value(e->eval(exec_con)));
    }

    return {ValueType::List, result};
}

Value VariableExpr::eval(ExecutionContext& exec_con) {
    return exec_con.env.get(name, loc);
}

Value BinaryExpr::eval(ExecutionContext& exec_con) {
    Value va = left->eval(exec_con);
    Value vb = right->eval(exec_con);

    return Operations::applyOp(op, va, vb, loc);
}

Value CallExpr::eval(ExecutionContext& exec_con) {
    if (natives.find(callee) != natives.end()) {
        return natives[callee](exec_con, args, loc);
    }

    if (exec_con.env.exists_func(callee)) {
        auto func = exec_con.env.get_func(callee, loc);

        if (args.size() != func.args.size()) {
            throw CompileError("Function " + callee + " expects " + std::to_string(func.args.size()) + " arguments, got " + std::to_string(args.size()), loc);
        }

        Environment funEnv(&exec_con.env);
        ExecutionContext funCont{funEnv, exec_con.runtime};

        for (size_t i = 0; i<func.args.size(); i++) {
            funCont.env.declare(func.args[i], args[i]->eval(exec_con).type, args[i]->eval(exec_con), loc);
        }

        for (auto& i : *func.body) {
            i->execute(funCont);
        }

        return {ValueType::Undefined, std::monostate{}};
    }

    throw RuntimeError("Undefined: " + callee, loc);
}

Value MemberCallExpr::eval(ExecutionContext& exec_con) {
    auto obj = object->eval(exec_con);

    if (allMethods.find(method) != allMethods.end()) {
        return allMethods[method](exec_con, obj, args, loc);
    }

    if (obj.type == ValueType::List) {
        if (listMethods.find(method) != listMethods.end()) {
            return listMethods[method](exec_con, obj, args, loc);
        }
    }

    throw RuntimeError(typeToStr(obj.type) + " has no method called " + method, loc);
}

Value ModuleCallExpr::eval(ExecutionContext& exec_con) {
    Value obj = object->eval(exec_con);

    if (obj.type != ValueType::Module) {
        throw RuntimeError("Cannot access property of " + typeToStr(obj.type), loc);
    }

    auto module = std::get<std::shared_ptr<Module>>(obj.data);

    if (!module) {
        throw RuntimeError("Invalid module", loc);
    }


    if (module->getEnv().exists_func(method)) {

        auto func = module->getEnv().get_func(method);

        if (args.size() != func.args.size()) {
            throw RuntimeError(method + " excpected " + std::to_string(func.args.size()) + ", got " + std::to_string(args.size()), loc);
        }

        Environment funcEnv(&module->getEnv());
        ExecutionContext funCont{funcEnv, exec_con.runtime};

        for (size_t i = 0; i<func.args.size(); i++) {
            Value arg = args[i]->eval(exec_con);
            funCont.env.declare(func.args[i], arg.type, arg, loc);
        }

        for (auto& i : *func.body) {
            i->execute(funCont);
        }

        return {ValueType::Undefined, std::monostate{}};
    }

    throw RuntimeError("Undefined: " + method, loc);
}

Value GetExpr::eval(ExecutionContext& exec_con) {

    Value obj = object->eval(exec_con);

    if (obj.type != ValueType::Module) {
        throw RuntimeError("Cannot access property of " + typeToStr(obj.type), loc);
    }

    auto module = std::get<std::shared_ptr<Module>>(obj.data);

    if (!module) {
        throw RuntimeError("Invalid module", loc);
    }

    return module->getEnv().get(name, loc);
}

Value IndexExpr::eval(ExecutionContext& exec_con) {
    Value listValue = object->eval(exec_con);
    Value listIndex = index->eval(exec_con);

    if (!(listValue.type == ValueType::List || listValue.type == ValueType::String)) {
        throw RuntimeError("Cannot index " + typeToStr(listValue.type), loc);
    }

    if (listIndex.type != ValueType::Int) {
        throw RuntimeError("Index must be int", loc);
    }

    if (listValue.type == ValueType::List) {
        auto& list = std::get<List>(listValue.data);
        int idx = std::get<int>(listIndex.data);

        if (idx == -1) {
            idx = static_cast<int>(list->size())-1;
        }

        if (idx < 0 || idx >= static_cast<int>(list->size())) {
            throw RuntimeError("Index out of range", loc);
        }

        return (*list)[static_cast<size_t>(idx)];
    }

    if (listValue.type == ValueType::String) {
        auto& string = std::get<std::string>(listValue.data);
        int idx = std::get<int>(listIndex.data);

        if (idx == -1) {
            idx = static_cast<int>(string.length());
        }

        if (idx < 0 || idx >= int(string.length())) {
            throw RuntimeError("Index out of range", loc);
        }

        return {ValueType::String, std::string(1, string[static_cast<size_t>(idx)])};
    }

    throw RuntimeError("Something went wrong indexing an object", loc);
}

void VarStmt::execute(ExecutionContext& exec_con) {
    Value v = value->eval(exec_con);

    if (declaredType == ValueType::Undefined) {
        declaredType = v.type;
    }

    if (v.type != declaredType) {
        if (v.type == ValueType::Int && declaredType == ValueType::Float) {
            v = {ValueType::Float, static_cast<double>(std::get<int>(v.data))};
        } else if (v.type == ValueType::Float && declaredType == ValueType::Int) {
            v = {ValueType::Int, static_cast<int>(std::get<double>(v.data))};
        } else {
            throw TypeError("Expected '" + typeToStr(declaredType) + "', got '" + typeToStr(v.type) + "'", loc);
        }
    }

    exec_con.env.declare(name, declaredType, v, loc);
}

void ExpressionStmt::execute(ExecutionContext& exec_con) {
    std::vector<Value> evaluated;

    for (auto& expr : values) {
        evaluated.push_back(expr->eval(exec_con));
    }

    if (!exec_con.env.replMode)
        return;

    bool first = true;

    for (size_t i = 0; i < evaluated.size(); i++) {
        if (evaluated[i].type == ValueType::Undefined)
            continue;

        if (!first)
            std::cout << " ";

        first = false;

        std::cout << toString(
            evaluated[i],
            values[i]->loc
        );
    }

    if (!first)
        std::cout << std::endl;
}

void AssignStmt::execute(ExecutionContext& exec_con) {
    exec_con.env.set(name, value->eval(exec_con), loc);
}

void DeleteStmt::execute(ExecutionContext& exec_con) {
    for (auto &name : names)
        exec_con.env.remove(name, loc);
}

void FunStmt::execute(ExecutionContext& exec_con) {
    exec_con.env.new_func(name, type, args, std::make_unique<std::vector<std::unique_ptr<Stmt>>>(std::move(body)), loc);
}

void IfStmt::execute(ExecutionContext& exec_con) {
    Value cond = condition->eval(exec_con);

    if (cond.isTruthy()) {
        for (auto& stmt : thenBranch) {
            stmt->execute(exec_con);
        }
    } else {
        for (auto& stmt : elseBranch) {
            stmt->execute(exec_con);
        }
    }
}

void WhileStmt::execute(ExecutionContext& exec_con) {
    while (true) {
        Value cond = condition->eval(exec_con);

        if (!cond.isTruthy()) {
            break;
        }

        for (auto& stmt : body) {
            stmt->execute(exec_con);
        }
    }
}

void ForStmt::execute(ExecutionContext& exec_con) {
    Value iterableValue = iterable->eval(exec_con);
    auto& list = std::get<List>(iterableValue.data);

    if (exec_con.env.exists(variable)) {
        exec_con.env.remove(variable, loc);
    }

    for (auto& item : *list) {
        Environment loopEnv(&exec_con.env);
        ExecutionContext loopCont{loopEnv, exec_con.runtime};

        loopCont.env.declare(variable, item.type, item);

        for (auto& stmt : body) {
            stmt->execute(loopCont);
        }
    }
}

void ImportStmt::execute(ExecutionContext& exec_con) {
    for (auto& name : names) {
        fs::path path = name;

        std::cout << path;

        std::cout << (exec_con.runtime.loadModule(path, loc)?"Ano":"Ne");

        std::string moduleName = path.stem().string();

        auto it = exec_con.runtime.modules.find(moduleName);

        if (it == exec_con.runtime.modules.end()) {
            throw RuntimeError("Module not found: " + moduleName, loc);
        }

        Value module{ValueType::Module, it->second};
        exec_con.env.declare(moduleName, ValueType::Module, module, loc);
    }
}
