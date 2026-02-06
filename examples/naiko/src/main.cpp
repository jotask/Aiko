#include <magic_enum/magic_enum.hpp>

#include <compiler_types.h>
#include <compiler_helper.h>
#include <preprocessor.h>
#include <lexer.h>
#include <parser.h>
#include <emitter/emitter.h>
#include <naiko_compiler_options.h>

#include <logger/logger.h>

#include "semantic_analyzer.h"
#include "emitter/cpp/cpp_emitter.h"
#include "emitter/llvm/llvm_emitter.h"

static aiko::naiko::CompilerOptions generateDEBUGCompilerOptions(int argc, char ** argv)
{
	const aiko::string woorkingDirectory = "/home/jose/Projects/examples/naiko/src/";
	const aiko::naiko::CompilerOptions opts =
	{
		.inputFiles = {{woorkingDirectory + "brainfuck.naiko"}},
		.outputFile = aiko::string(woorkingDirectory + "brainfuck"),
		.emitterKind = aiko::naiko::CompilerOptions::EmitterKind::LLVM,
	};
	aiko::naiko::checkMandatoryCompilerOptions(opts);
	return opts;
}

int main(int argc, char** argv)
{

	// aiko::naiko::CompilerOptions opts = aiko::naiko::parseArgs(argc, argv);
	aiko::naiko::CompilerOptions opts = generateDEBUGCompilerOptions(argc, argv);

	// ###########
	//    PREPROCESSOR
	// ###########

	aiko::naiko::Preprocessor preprocessor(opts.inputFiles);
	aiko::string code = preprocessor.parse();

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
	//    SEMANTIC ANALYZER
	// ############

	aiko::logger::Log::info("Semantic Analyzer started");

	aiko::naiko::SemanticAnalyzer analyzer;
	analyzer.analyze(ast.get());

	aiko::logger::Log::info("Semantic Analyzer completed");

	// ############
	//    EMITTER
	// ############

	aiko::logger::Log::info("Emitter Started");

	aiko::AikoUPtr<aiko::naiko::Emitter> emitter;

	switch (opts.emitterKind)
	{
		case aiko::naiko::CompilerOptions::EmitterKind::CPP: emitter = std::make_unique<aiko::naiko::CppEmitter>(opts); break;
		case aiko::naiko::CompilerOptions::EmitterKind::LLVM: emitter = std::make_unique<aiko::naiko::LlvmEmitter>(opts); break;
		default:
			aiko::logger::Log::error("Unsuportted emitter mode");
			std::exit(EXIT_FAILURE);
	}

	emitter->emit(ast.get());
	aiko::logger::Log::info("Emitter completed...");

	aiko::logger::Log::info("Compilation started");
	emitter->compile();
	aiko::logger::Log::info("Compilation copleted...");

	return EXIT_SUCCESS;
}
