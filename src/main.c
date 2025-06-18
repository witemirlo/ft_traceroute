#include "ft_traceroute.h"

const uint8_t max_hops = 30;

void dump(void const* const buffer, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%x ", ((uint8_t*)buffer)[i]);
	printf("\n");
}


int main(int argc, char* argv[])
{
	t_connection_data data[max_hops];
	const char*       addr = NULL;
	const char        payload[32] = {0}; // TODO: poner otro payload
	char              buffer[BUFSIZ];

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

	struct addrinfo hints = {0};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_UDP;

	for (uint8_t i = 0; i < max_hops; i++)
		get_connection_data(data + i, addr, &hints);

	// for (uint8_t i = 0; i < max_hops; i++) {
	// 	if (sendto(data[i].sockfd, "hi", 2, 0, (struct sockaddr*)&(data[i].addr), data[i].addr_len) < 0)
	// 		fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
	// }

	fd_set main_set, write_set, read_set;
	struct timeval tv = {0};

	FD_ZERO(&main_set);;
	for (uint8_t i = 0; i < max_hops; i++)
		FD_SET(data[i].sockfd, &main_set);
	
	int8_t pending = max_hops;
	while (1) {
		write_set = main_set;
		read_set = main_set;

		if (select(data[max_hops - 1].sockfd + 1, &read_set, &write_set, NULL, &tv) < 0) {
			// TODO: CONTROL DE ERRORES
		}

		for (uint8_t i = 0; i < max_hops; i++) {
			t_connection_data* data_pt = data + i;

			if (pending <= 0)
				break;
			if (FD_ISSET(data_pt->sockfd, &write_set)) {
				// TODO: enviar paquete si no se han enviado mas de tres
				if (data_pt->packets_sent < 3) {
					if (sendto(data_pt->sockfd, payload, sizeof(payload), 0, (struct sockaddr*)&(data_pt->addr), data_pt->addr_len) < 0) {
						// TODO: control de errores
						fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
					}
					data_pt->packets_sent++;
				}
			}

			if (FD_ISSET(data_pt->sockfd, &read_set)) {
				// TODO: leer el paquete
				// TODO: que no edite el sockaddr original
				ssize_t ret;
				struct sockaddr_in tmp = data_pt->addr; // TODO: renombrar
				socklen_t tmp2 = sizeof(tmp);

				if ((ret = recvfrom(data_pt->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&(tmp), &tmp2)) < 0) { // TODO: control de errores
					fprintf(stderr, "%s:%d:\tdata_pt->sockfd(%d): %s\n", __FILE__, __LINE__, data_pt->sockfd, strerror(errno)); // TODO: borrar
					// ret = recvfrom(data_pt->sockfd, buffer, sizeof(buffer), MSG_ERRQUEUE, (struct sockaddr*)&(data_pt->addr), &data_pt->addr_len); // TODO: esto no está funcionando, pero si que retorna el tamano del paquete
				}
				data_pt->packets_received++;
				fprintf(stderr, "%s:%d:\tdata_pt->sockfd(%d): %2ld: %s\n", __FILE__, __LINE__, data_pt->sockfd, ret, inet_ntoa(tmp.sin_addr)); // TODO: borrar
				// TODO: comprobar que el paquete corresponde con los enviados (id, payload...)
				// TODO: comprobar el patete recibido para ver si ya ha terminado
			}

			if (data_pt->packets_received == 3) { // TODO: macro o variable para este numero
				// TODO: imprimir la informacion
				// TODO: 74.125.37.87 (74.125.37.87)  4.696 ms 142.250.232.7 (142.250.232.7)  5.566 ms 74.125.37.87 (74.125.37.87)  8.569 ms, puede que los paquetes llegen de ips diferentes
				// TODO: eliminar de main_set
				FD_CLR(data_pt->sockfd, &main_set);
				pending--;
			}
		}

		// TODO: si ya ha terminado salir del bucle
		if (pending <= 0)
			break;
	}
	
	for (uint8_t i = 0; i < max_hops; i++)
		destroy_connection_data(data + i);

	return EXIT_SUCCESS;
}
