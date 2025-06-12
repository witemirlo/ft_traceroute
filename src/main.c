#include "ft_traceroute.h"

const uint8_t max_hops = 30;

int main(int argc, char* argv[])
{
	t_connection_data data[max_hops];
	const char*       addr = NULL;

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

	ft_memset(&data, 0, sizeof(t_connection_data) * max_hops);

	for (uint8_t i = 0; i < max_hops; i++)
		get_connection_data(data + i, addr);

	for (uint8_t i = 0; i < max_hops; i++) {
		if (sendto(data[i].sockfd, "hi", 2, 0, (struct sockaddr*)&(data[i].addr), data[i].addr_len) < 0)
			fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, strerror(errno));
	}

	// fd_set main_set, write_set, read_set;
	// struct timeval tv = {0};

	// FD_ZERO(&main_set);;
	// for (uint8_t i = 0; i < max_hops; i++)
	// 	FD_SET(data[i].sockfd, &main_set);

	// while (1) {
	// 	write_set = main_set;
	// 	read_set = main_set;

	// 	if (select(data[max_hops - 1].sockfd + 1, &read_set, &write_set, NULL, &tv) < 0) {
	// 		// TODO: CONTROL DE ERRORES
	// 	}

	// 	for (uint8_t i = 0; i < max_hops; i++) {
	// 		t_connection_data const* data_pt = data + i;

	// 		if (FD_ISSET(data_pt->sockfd, &write_set)) {
	// 			// TODO: enviar paquete si no se han enviado mas de tres
	// 		}

	// 		if (FD_ISSET(data_pt->sockfd, &read_set)) {
	// 			// TODO: leer el paquete
	// 		}

	// 		if (data_pt->packets_received == 3) { // TODO: macro o variable para este numero
	// 			// TODO: imprimir la informacion
	// 			// TODO: eliminar de main_set
	// 		}
	// 	}

	// 	// TODO: si ya ha terminado salir del bucle
	// }
	
	for (uint8_t i = 0; i < max_hops; i++)
		destroy_connection_data(data + i);

	return EXIT_SUCCESS;
}
