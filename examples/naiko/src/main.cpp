#include <stdlib.h>

#include <aiko.h>
#include <core/file.h>

#include <magic_enum/magic_enum.hpp>

#include <compiler_types.h>
#include <compiler_helper.h>
#include <lexer.h>
#include <parser.h>
#include <emitter.h>

#include "constants.h"

#include <logger/logger.h>

int main()
{
	const aiko::string fullPath = naiko::global::getAssetPath("naiko.naiko");
	const aiko::string code = aiko::files::readFileContent(fullPath.c_str());

	aiko::naiko::Tokenization tokenizator;

	// ###########
	//    LEXER
	// ###########

	aiko::naiko::Lexer lexer(code);

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

	// ############
	//    PARSER
	// ############

	aiko::naiko::Parser emitter(tokenizator);

	// ############
	//    EMITTER
	// ############

	// TODO

	return EXIT_SUCCESS;
}