// module.cpp

#include "module.hpp"
#include "runtime/environment.hpp"
#include "runtime/value.hpp"

Module::Module(
    const std::string& name,
    const std::filesystem::path& path,
    Environment env
) : n(name), p(path), e(std::move(env)) {}

const std::string& Module::getName() const {
    return n;
}

const std::filesystem::path& Module::getPath() const {
    return p;
}

Environment& Module::getEnv() {
    return e;
}
