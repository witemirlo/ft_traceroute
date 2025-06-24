#include "ft_traceroute.h"
#include <netinet/udp.h>
#include <stdint.h>

const uint8_t max_hops = 30;

void dump(void const* const buffer, size_t size)
{
	for (size_t i = 0; i < size; i++)
		printf("%x ", ((uint8_t*)buffer)[i]);
	printf("\n");
}

int main(int argc, char* argv[])
{
	t_connection_data data;
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

	fd_set main_set;
	struct timeval tv;
	
	int8_t packets_sent = 0;
	while (packets_sent < max_hops) {
		get_connection_data(&data, addr, &hints);

		for (int8_t n_packet = 0; n_packet < 3; n_packet++) {
			// TODO: wip

			FD_ZERO(&main_set);;
			FD_SET(data.sockfd, &main_set);
			if (select(data.sockfd + 1, NULL, &main_set, NULL, NULL) < 0) {
				continue;
			}

			if (FD_ISSET(data.sockfd, &main_set)) {
				// TODO: enviar paquete si no se han enviado mas de tres
				if (packets_sent < 3) {
					if (sendto(data.sockfd, payload, sizeof(payload), 0, (struct sockaddr*)&(data.addr), data.addr_len) < 0) {
						// TODO: control de errores
						fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
					}
					packets_sent++;
				}
			}

			tv.tv_sec = 1; // TODO: mirar el timeout real
			tv.tv_usec = 0;
			FD_ZERO(&main_set);;
			FD_SET(data.sockfd, &main_set);
			if (select(data.sockfd + 1, &main_set, NULL, NULL, &tv) < 0) {
				// TODO: poner el timeout en tv
				// TODO: el paquete no ha llegado, poner *
			}
			if (FD_ISSET(data.sockfd, &main_set)) {
				// TODO: leer el paquete
				// TODO: que no edite el sockaddr original
				ssize_t ret;
				struct sockaddr_in tmp = data.addr; // TODO: renombrar
				socklen_t tmp2 = sizeof(tmp);

				if ((ret = recvfrom(data.sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&(tmp), &tmp2)) < 0) { // TODO: control de errores
					fprintf(stderr, "%s:%d:\tsockfd(%d): %s\n", __FILE__, __LINE__, data.sockfd, strerror(errno)); // TODO: borrar
					// ret = recvfrom(data.sockfd, buffer, sizeof(buffer), MSG_ERRQUEUE, (struct sockaddr*)&(addr), &addr_len); // TODO: esto no está funcionando, pero si que retorna el tamano del paquete
				}
				fprintf(stderr, "%s:%d:\tsockfd(%d): %2ld: %s\n",
					__FILE__, __LINE__,
					data.sockfd,
					ret,
					inet_ntoa(((struct ip*)buffer)->ip_src)
				); // TODO: borrar
				// TODO: comprobar que el paquete corresponde con los enviados (id, payload...)
				// TODO: comprobar el patete recibido para ver si ya ha terminado
			}
			// TODO: el else con el * de que no ha llegado en el tiempo
		}
		destroy_connection_data(&data);
		// TODO: si ya ha terminado salir del bucle
	}
	
	return EXIT_SUCCESS;
}
