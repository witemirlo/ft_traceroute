#include "ft_traceroute.h"

static void
usage_and_exit(FILE* stream, int code)
{
	fprintf(stream,
		"Usage:\n"
		"  ft_traceroute host\n"
		"Options:\n"
		"  --help            Read this help and exit\n"
		"\n"
		"Arguments:\n"
		"+     host          The host to traceroute to\n"
	);

	exit(code);
}

char const*
init(int argc, char *argv[])
{
	char const* addr = NULL;

	for (int i = 1; i < argc; i++) {
		if (ft_strcmp(argv[i], "--help") == 0 ||
		    addr != NULL)
			usage_and_exit(stderr, EXIT_FAILURE);

		if (addr == NULL)
			addr = argv[i];
	}

	if (addr == NULL)
		usage_and_exit(stdout, EXIT_SUCCESS);

	return addr;
}
