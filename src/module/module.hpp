// module.hpp

#include <filesystem>

#include "runtime/environment.hpp"

class Module {
private:
    const std::string& n;
    const std::filesystem::path& p;
    Environment e;

public:
    Module(
        const std::string& name,
        const std::filesystem::path& path,
        Environment env
    );

    const std::string& getName() const;
    const std::filesystem::path& getPath() const;

    Environment& getEnv();
};
