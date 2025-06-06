#include "ft_traceroute.h"

int main(int argc, char* argv[])
{
	t_connection_data data = {0};
	const char* addr = NULL;

	for (int i = 1; i < argc; i++) {
		if (ft_strcmp(argv[i], "--help") == 0) {
			printf(
				"Usage:\n"
				"  %s host\n"
				"Options:\n"
				"  --help                      Read this help and exit\n"
				"\n"
				"Arguments:\n"
				"+     host          The host to traceroute to\n"
				, __progname
			);
			return EXIT_SUCCESS;
		}
		if (addr == NULL)
			addr = argv[i];
		else {
			fprintf(stderr, "%s: Error: Bad option `%s'\n", __progname, argv[i]);
			return EXIT_FAILURE;
		}
	}

	get_connection_data(&data, addr);
	destroy_connection_data(&data);
	return EXIT_SUCCESS;
}
