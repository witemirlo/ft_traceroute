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

static int
parse_num(char const* const str)
{
        for (size_t i = 0; str[i]; i++) {
                if (ft_isdigit(str[i]))
                        continue;

                fprintf(stderr, "ft_traceroute: invalid value (`%s' near `%s')\n", str, str + i);
                return -1;
        }

        return ft_atoi(str);
}

void case_first(char const* const str)
{
	int n;

	if ((n = parse_num(str)) < 0)
		exit(EXIT_FAILURE);

	if (n < 1)
		exit(EXIT_FAILURE);

	if (n > max_hops)
		exit(EXIT_FAILURE);
}

char const*
init(int argc, char *argv[])
{
	char const* addr = NULL;

	for (int i = 1; i < argc; i++) {
		if (ft_strcmp(argv[i], "--help") == 0 ||
		    addr != NULL)
			usage_and_exit(stderr, EXIT_FAILURE);

		else if (ft_strncmp(argv[i], "--first=", ft_strlen("--first=")))
			case_first(argv[i] + ft_strlen("--first="));

		if (addr == NULL)
			addr = argv[i];
	}

	if (addr == NULL)
		usage_and_exit(stdout, EXIT_SUCCESS);

	return addr;
}
