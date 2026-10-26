// environment.cpp

#include "runtime/environment.hpp"
#include "common/error.hpp"
#include "runtime/runtime.hpp"
#include <error.h>

Environment::Environment(
    Environment* p
)
: parent(p) {}

void Environment::declare(const std::string& name, ValueType type, Value value, SourceLocation loc) {
    if (exists_local(name)) {
		throw RuntimeError("Variable already exists: " + name, loc);
	} else if (exists_func_local(name)) {
		throw RuntimeError("There is already a function called " + name, loc);
	}

	vars[name] = {type, value};
}

void Environment::set(const std::string& name, Value value, SourceLocation loc) {

    auto it = vars.find(name);

    if (it != vars.end()) {
        if (it->second.type != value.type) {
            throw ValueError("Type mismatch", loc);
        }

        it->second.value = value;
        return;
    }

    if (parent) {
        parent->set(name, value, loc);
        return;
    }

	throw RuntimeError("Undefined: " + name, loc);
}

Value Environment::get(const std::string& name, SourceLocation loc) {
    if (exists_func(name)) {
        throw RuntimeError("Expected arguments in function call", loc);
    }

    auto it = vars.find(name);

    if (it != vars.end()) {
        return it->second.value;
    }

    if (parent) {
        return parent->get(name, loc);
    }

	throw RuntimeError("Undefined: " + name, loc);
}

bool Environment::exists(const std::string& name) {
    if (vars.find(name) != vars.end()) {
        return true;
    }
    if (parent) {
        return parent->exists(name);
    }

    return false;
}

bool Environment::exists_local(const std::string& name) {
    return vars.find(name) != vars.end();
}

void Environment::remove(const std::string& name, SourceLocation loc) {
	if (!exists(name)) {
		throw RuntimeError("Undefined: " + name, loc);
	}

	vars.erase(name);
}

bool Environment::exists_func(const std::string& name) {
    if (funcs.find(name) != funcs.end()) {
        return true;
    }

    if (parent) {
        return parent->exists_func(name);
    }

    return false;
}

bool Environment::exists_func_local(const std::string& name) {
    return funcs.find(name) != funcs.end();
}

void Environment::new_func(
    const std::string& name,
    ValueType type,
    std::vector<std::string> args,
    std::unique_ptr<std::vector<std::unique_ptr<Stmt>>> body,
    SourceLocation loc
) {
	if (exists_func(name)) {
		throw RuntimeError("Function already exists " + name, loc);
	} else if (exists(name)) {
		throw RuntimeError("There is already a variable called " + name, loc);
	}

	funcs[name] = {
		std::move(type),
		std::move(args),
		std::move(body)
	};
}

const Function_to_execute Environment::get_func(const std::string& name, SourceLocation loc) {
	if (!exists_func(name)) {
		throw RuntimeError("Function " + name + " does not exists", loc);
	}

	return Function_to_execute{funcs[name].return_type, funcs[name].args, funcs[name].body};
}
