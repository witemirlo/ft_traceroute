#include "ft_traceroute.h"

int main(int argc, char* argv[])
{
	t_connection_data data = {0};
	const char* addr;

	for (int i = 1; i < argc; i++) {
		if (ft_strcmp(argv[i], "--help") == 0) {
			printf("TODO: help\n"); // TODO: poner el outuput que toca
			return EXIT_SUCCESS;
		}
		addr = argv[i];
		break;
	}

	get_connection_data(&data, addr);
	destroy_connection_data(&data);
	return EXIT_SUCCESS;
}
