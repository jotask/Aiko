#include <catch2/catch_test_macros.hpp>

#include "lexer.h"
#include "parser.h"
#include "compiler_helper.h"
#include "emitter/emitter.h"
#include "emitter/cpp/cpp_emitter.h"

#include "test_types.h"
#include "tests_code.h"

#include "constants.h"

#define TEST_LOCATION_RELATIVE_PATH "build/"

static void compileCmd(const aiko::string& name, const aiko::string& cmd)
{
    int result = std::system(cmd.c_str());
    if (result != EXIT_SUCCESS)
    {
        // On POSIX systems, exit code is in the high byte
        const int exitCode = WEXITSTATUS(result);
        aiko::logger::Log::info("Exit code: [%d]", exitCode);
        aiko::logger::Log::critical("Couldn't compile emitted code exited with error code [%d] -> [%s] -> [%s]", exitCode, name.c_str(), cmd.c_str());
    }
    REQUIRE(result == EXIT_SUCCESS);
}

static void runProgramAndCaptureOutput(const aiko::string& name)
{
    const aiko::string cmd = "./" + name + " 2>&1"; // capture stdout + stderr
    std::array<char, 256> buffer;
    std::string output;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        aiko::logger::Log::critical("Couldn't run program [%s] -> [%s]", name.c_str(), cmd.c_str());
        REQUIRE(false);
        return;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        output += buffer.data();

    const int result = pclose(pipe.release());
    const int exitCode = WEXITSTATUS(result);

    aiko::logger::Log::info("Output: %s", output.c_str());

    REQUIRE(exitCode == 0);

}

static void runProgram(const aiko::string& testName)
{
    // Compile
    {
        const aiko::string cmd = "g++ " + aiko::global::GLOBAL_PATH + TEST_LOCATION_RELATIVE_PATH + testName + ".cpp -o " + testName;
        compileCmd(testName, cmd);
    }
    // Execute it
    {
        runProgramAndCaptureOutput(testName);
    }
    aiko::logger::Log::info("Test completed [%s]", testName.c_str() );
}

#define AIKO_TEST(code_var, description)                                                                                            \
    TEST_CASE(description, "[EMITTER]" )                                                                                            \
    {                                                                                                                               \
        const char* code = code_var;                                                                                                \
                                                                                                                                    \
        INIT_LEXER                                                                                                                  \
                                                                                                                                    \
        printToken(token);                                                                                                          \
        while (token.kind != TokenKind::END)                                                                                        \
        {                                                                                                                           \
            token = lex.next();                                                                                                     \
            printToken(token);                                                                                                      \
            tokens.push_back(token);                                                                                                \
        }                                                                                                                           \
        auto parser = Parser(tokens);                                                                                               \
        auto ast = parser.program();                                                                                                \
        ast->print();                                                                                                               \
                                                                                                                                    \
        const std::string path = aiko::global::GLOBAL_PATH + TEST_LOCATION_RELATIVE_PATH + std::string(#code_var) + ".cpp";         \
        aiko::logger::Log::info(path.c_str() );                                                                                     \
        aiko::AikoUPtr<Emitter> emitter = std::make_unique<CppEmitter>(path);                                                       \
        emitter->emit(ast.get());                                                                                                   \
                                                                                                                                    \
        runProgram(std::string(#code_var));                                                                                         \
                                                                                                                                    \
    }                                                                                                                               \

AIKO_TEST(test_code_print_correct_identifier, "Emitter correct indentifiers")
AIKO_TEST(test_code_print_string, "Emitter Hello World string")
AIKO_TEST(test_code_print_digit, "Emitter Hello World number")
AIKO_TEST(test_code_print_multiple_print, "Emitter Multiple prints")
AIKO_TEST(test_code_basic_if, "Emitter Basic if")
AIKO_TEST(test_code_basic_operations, "Emitter Basic operations")
AIKO_TEST(test_code_big_code, "Emitter Big code")
