#include <stdlib.h>

#include <aiko.h>
#include <core/file.h>

#include <magic_enum/magic_enum.hpp>

#include <compiler_types.h>
#include <compiler_helper.h>
#include <lexer.h>

#include "constants.h"

#include <logger/logger.h>

int main()
{
	const aiko::string fullPath = naiko::global::getAssetPath("naiko.naiko");
	const aiko::string code = aiko::files::readFileContent(fullPath.c_str());

	aiko::naiko::Lexer lexer(code);

	aiko::naiko::Token token = lexer.next();
	aiko::naiko::printToken(token);
	while (token.kind != aiko::naiko::TokenKind::END)
	{
		token = lexer.next();
		aiko::naiko::printToken(token);
	}

	return EXIT_SUCCESS;
}