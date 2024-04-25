#include <stdlib.h>

#include "nes6502.h"

int main()
{
	nes::nes6502 nes;
	nes.run();
	return EXIT_SUCCESS;
}