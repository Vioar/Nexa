// runtime.hpp

#pragma once

#include "common/error.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <string>

class Environment;
class Module;

class Runtime {
public:
    std::shared_ptr<Environment> globalEnv;

    std::unordered_map<
        std::string,
        std::shared_ptr<Module>
    > modules;

    bool unloadModule(const std::string& name, SourceLocation loc);
    bool moduleLoaded(const std::string& name, SourceLocation loc);
    bool loadModule(const std::filesystem::path& path, SourceLocation loc);

    Runtime();
};
