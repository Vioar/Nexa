// main.cpp

#include <iostream>
#include <fstream>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "ast/ast.hpp"
#include "runtime/environment.hpp"
#include "runtime/runtime.hpp"
#include "common/error.hpp"
#include "runtime/execution_context.hpp"

int main(int argc, char** argv) {
    Environment env;
    Runtime runtime;
    ExecutionContext exe_cont{env, runtime};
    std::string line;

    if (argc > 1) {
        try {
            std::ifstream file(argv[1], std::ios_base::in);

            if (!file.is_open())
                throw std::runtime_error("Cannot open file " + std::string(argv[1]));

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
                    stmt->execute(exe_cont);
                } catch (const std::exception& e) {
                    std::cerr << "[runtime error] " << e.what() << std::endl;
                }
            }

            return 0;
        } catch (const CompileError& e) {
            std::cerr << "[compile error] "
                    << e.what()
                    << " at " << e.loc.line << ":" << e.loc.column
                    << std::endl;
        }
        catch (const RuntimeError& e) {
            std::cerr << "[runtime error] "
                    << e.what()
                    << " at " << e.loc.line << ":" << e.loc.column
                    << std::endl;
        }
        catch (const ValueError& e) {
            std::cerr << "[Value error] "
                    << e.what()
                    << " at " << e.loc.line << ":" << e.loc.column
                    << std::endl;
        }
        catch (const TypeError& e) {
            std::cerr << "[Type error] "
                    << e.what()
                    << " at " << e.loc.line << ":" << e.loc.column
                    << std::endl;
        }
        catch (const MathError& e) {
            std::cerr << "[Math error] "
                    << e.what()
                    << " at " << e.loc.line << ":" << e.loc.column
                    << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }

    } else {
        while (true) {

            env.replMode = true;

            std::cout << ">>: ";
            std::getline(std::cin, line);

            try {
                if (line == "exit" || line == "quit") break;

                Lexer lexer(line);
                auto tokens = lexer.tokenize();

                Parser parser(tokens, true);
                auto program = parser.parse();

                for (auto& stmt : program) {
                    try {
                        stmt->execute(exe_cont);
                    } catch (const std::exception& e) {
                        std::cerr << "[runtime error] " << e.what() << std::endl;
                    }
                }

            } catch (const CompileError& e) {
                std::cerr << "[compile error] "
                        << e.what()
                        << " at " << e.loc.line << ":" << e.loc.column
                        << std::endl;
            }
            catch (const RuntimeError& e) {
                std::cerr << "[runtime error] "
                        << e.what()
                        << " at " << e.loc.line << ":" << e.loc.column
                        << std::endl;
            }
            catch (const ValueError& e) {
                std::cerr << "[Value error] "
                        << e.what()
                        << " at " << e.loc.line << ":" << e.loc.column
                        << std::endl;
            }
            catch (const TypeError& e) {
                std::cerr << "[Type error] "
                        << e.what()
                        << " at " << e.loc.line << ":" << e.loc.column
                        << std::endl;
            }
            catch (const MathError& e) {
                std::cerr << "[Math error] "
                        << e.what()
                        << " at " << e.loc.line << ":" << e.loc.column
                        << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}
