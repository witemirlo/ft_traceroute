#include "ft_traceroute.h"

int main(int argc, char* argv[])
{
	const char* const addr = init(argc, argv);
	t_connection_data data = {0};

	routine(&data, addr);

	return EXIT_SUCCESS;
}
