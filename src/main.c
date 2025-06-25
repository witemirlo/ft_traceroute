#include "ft_traceroute.h"
#include <sys/select.h>

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

	ft_memset(&data, 0, sizeof(t_connection_data));

	struct addrinfo hints = {0};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_UDP;

	struct timeval tv;
	fd_set main_set, write_set, read_set;
	FD_ZERO(&main_set);;
	FD_SET(data.sockfd, &main_set);

	struct ip* ip_ptr = (struct ip*)buffer;
	struct icmp* icmp_ptr = (struct icmp*)(buffer + sizeof(struct ip));
	
	// setbuf(stdout, NULL); // TODO: borrar
	// setbuf(stderr, NULL); // TODO: borrar

	FD_ZERO(&write_set);
	FD_ZERO(&read_set);
	for (uint8_t ttl_round = 1; ttl_round <= max_hops; ttl_round++) {
		printf("%2d", ttl_round);
		get_connection_data(&data, addr, &hints);

		for (int8_t n_packet = 0; n_packet < 3; n_packet++) {
			// TODO: wip
			tv.tv_sec = 5; // TODO: mirar el timeout real
			tv.tv_usec = 0;
			
			FD_CLR(data.sockfd, &write_set);
			FD_CLR(data.sockfd, &read_set);

			FD_SET(data.sockfd, &write_set);
			FD_SET(data.sockfd, &read_set);

			if (select(data.sockfd + 1, NULL, &write_set, NULL, NULL) < 0) {
				continue;
			}

			if (FD_ISSET(data.sockfd, &write_set)) {
				t_packet packet;

				set_udp(&packet);
				if (sendto(data.sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&(data.addr), data.addr_len) < 0) {
					// TODO: control de errores
					fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
				}
			}

			if (select(data.sockfd + 1, &read_set, NULL, NULL, &tv) < 0) {
				// TODO: el paquete no ha llegado, poner *
			}
			if (FD_ISSET(data.sockfd, &read_set)) {
				// TODO: leer el paquete
				// TODO: que no edite el sockaddr original
				ssize_t ret;
				struct sockaddr_in tmp = data.addr; // TODO: renombrar
				socklen_t tmp2 = sizeof(tmp);
				ft_memset(buffer, 0, sizeof(buffer));

				if ((ret = recvfrom(data.sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&tmp, &tmp2)) < 0) { // TODO: control de errores
					fprintf(stderr, "%s:%d:\tsockfd(%d): %s\n", __FILE__, __LINE__, data.sockfd, strerror(errno)); // TODO: borrar
				}
				// TODO: sólo los que tengan el ttl expired
				// ((struct icmp*)(&buffer[sizeof(struct ip)]))->icmp_type

				printf("  %s",
					inet_ntoa(icmp_ptr->icmp_ip.ip_src)
				);

				// TODO: comprobar que el paquete corresponde con los enviados (id, payload...)
				// TODO: comprobar el patete recibido para ver si ya ha terminado
			} else {
				// TODO: el else con el * de que no ha llegado en el tiempo
				printf("  *");
			}
		}
		printf("\n");
		destroy_connection_data(&data);
		// TODO: si ya ha terminado salir del bucle
	}
	
	return EXIT_SUCCESS;
}
