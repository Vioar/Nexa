// runtime.cpp

#include <fstream>

#include "runtime/runtime.hpp"
#include "common/error.hpp"
#include "module/module.hpp"
#include "runtime/environment.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "runtime/execution_context.hpp"

Runtime::Runtime() {
    //globalEnv = std::make_shared<Environment>();
}

bool Runtime::moduleLoaded(const std::string& name, SourceLocation) {
    return modules.find(name) != modules.end();
}

bool Runtime::unloadModule(const std::string &name, SourceLocation loc) {
    if (modules.find(name) == modules.end()) {
        throw RuntimeError("Module not found: " + name, loc);
    }

    if (moduleLoaded(name, loc)) {
        modules.erase(name);
        return true;
    }
    return false;
}

bool Runtime::loadModule(const std::filesystem::path &path, SourceLocation loc) {
    std::string name = path.stem();

    if (moduleLoaded(name, loc)) {
        return false;
    }

    Environment env;
    ExecutionContext exec_cont{env, *this};

    std::ifstream file(path, std::ios_base::in);

    if (!file.is_open())
        throw RuntimeError("Cannot load module " + std::string(path), loc);

    env.replMode = false;

    std::string code(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens, false);
    auto program = parser.parse();

    for (auto& stmt : program) {
        try {
            stmt->execute(exec_cont);
        } catch (const std::exception& e) {
            throw RuntimeError("Problem while loading module " + name + ": " + e.what(), loc);
        }
    }

    modules[name] = std::make_shared<Module>(name, path, std::move(env));

    return true;
}
