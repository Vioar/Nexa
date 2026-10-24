// execution_context.hpp

#pragma once

class Environment;
class Runtime;

struct ExecutionContext {
    Environment& env;
    Runtime& runtime;
};
