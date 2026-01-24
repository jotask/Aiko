#include <stdlib.h>

#include <aiko.h>
#include <core/file.h>

#include <magic_enum/magic_enum.hpp>

#include <compiler_types.h>
#include <compiler_helper.h>
#include <lexer.h>
#include <parser.h>
#include <emitter/emitter.h>
#include <naiko_compiler_options.h>

#include "constants.h"

#include <logger/logger.h>

#include "emitter/cpp/cpp_emitter.h"


int main(int argc, char** argv)
{

	const aiko::naiko::CompilerOptions opts = aiko::naiko::parseArgs(argc, argv);

	const aiko::string code = aiko::files::readFileContent(opts.inputFiles.front().c_str());

	// ###########
	//    LEXER
	// ###########

	aiko::logger::Log::info("Lexer Started");

	aiko::naiko::Lexer lexer(code);
	aiko::naiko::Tokenization tokenizator;

	auto processToken = [&](const aiko::naiko::Token token)
	{
		tokenizator.push_back(token);
		aiko::naiko::printToken(token);
	};

	aiko::naiko::Token token = lexer.next();
	processToken(token);
	while (token.kind != aiko::naiko::TokenKind::END)
	{
		token = lexer.next();
		processToken(token);
	}

	aiko::logger::Log::info("Lexer completed...");

	// ############
	//    PARSER
	// ############

	aiko::logger::Log::info("Parser Started");

	aiko::naiko::Parser parser(tokenizator);
	auto ast = parser.program();

	aiko::logger::Log::info("Parser completed...");

	// ############
	//    EMITTER
	// ############

	aiko::logger::Log::info("Emitter Started");

	aiko::AikoUPtr<aiko::naiko::Emitter> emitter;

	switch (opts.emitterKind)
	{
		case aiko::naiko::CompilerOptions::EmitterKind::CPP: emitter = std::make_unique<aiko::naiko::CppEmitter>(opts.outputFile); break;
		default:
			aiko::logger::Log::error("Unsuportted emitter mode");
			std::exit(EXIT_FAILURE);
	}

	emitter->emit(ast.get());

	aiko::logger::Log::info("Emitter completed...");

	// ############
	//    Cpp compile
	// ############

	{
		const aiko::string cmd = "g++ " + opts.outputFile + ".cpp -o " + opts.outputFile;
		int result = std::system(cmd.c_str());
		if (result != EXIT_SUCCESS)
		{
			// On POSIX systems, exit code is in the high byte
			const int exitCode = WEXITSTATUS(result);
			aiko::logger::Log::info("Exit code: [%d]", exitCode);
			aiko::logger::Log::critical("Couldn't compile emitted code exited with error code [%d] -> [%s] -> [%s]", exitCode, opts.outputFile.c_str(), cmd.c_str());
		}

	}

	return EXIT_SUCCESS;
}
