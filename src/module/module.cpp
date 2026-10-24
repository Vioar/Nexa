// module.cpp

#include "module.hpp"
#include "runtime/environment.hpp"

Module::Module(
    const std::string& name,
    const std::filesystem::path& path,
    Environment env
) : n(name), p(path), e(std::move(env)) {}
