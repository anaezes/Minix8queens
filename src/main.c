#include "../lib/8queens.h"
#include "../lib/pixmap.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{

	sef_startup();

	if (argc == 1) {
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);

	return 0;
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"8queens\"\n",
			argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "8queens", strlen("8queens")) == 0)
	{

		if (argc != 2) {
			printf("8queens wrong no. of arguments for project()\n");
			return 1;
		}

		printf("8queens::\n");
		return game_loop();
	}

	else {
		printf("8queens: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

