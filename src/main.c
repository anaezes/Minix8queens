#include "8queens.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	sef_startup();

	return game_loop();
}
