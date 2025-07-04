#include "ft_traceroute.h"

const uint8_t max_hops = 30;

static void dump(void const* const buffer, size_t size)
{
	printf("\n");
	for (size_t i = 0; i < size; i++)
		printf("%x ", ((uint8_t*)buffer)[i]);
	printf("\n");
}

void routine(t_connection_data* const data, char const* const addr)
{
	struct timeval tv;
	struct addrinfo hints = {0};
	fd_set write_set, read_set;
	struct icmp packet;
	unsigned char     buffer[BUFSIZ];

	struct ip* ip_ptr = (struct ip*)buffer;
	struct icmp* icmp_ptr = (struct icmp*)(buffer + sizeof(struct ip));

	FD_ZERO(&write_set);
	FD_ZERO(&read_set);
	init_icmp(&packet);


	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	for (uint8_t ttl_round = 1; ttl_round <= max_hops; ttl_round++) {
		printf("%2d", ttl_round);
		get_connection_data(data, addr, &hints);

		for (int8_t n_packet = 0; n_packet < 1; n_packet++) { // TODO: cambiar a 3
			// TODO: wip
			tv.tv_sec = 5; // TODO: poner un 5
			tv.tv_usec = 0;
			
			FD_CLR(data->sockfd, &write_set);
			FD_CLR(data->sockfd, &read_set);

			FD_SET(data->sockfd, &write_set);
			FD_SET(data->sockfd, &read_set);

			if (select(data->sockfd + 1, NULL, &write_set, NULL, NULL) < 0) {
				continue;
			}

			if (FD_ISSET(data->sockfd, &write_set)) {
				// struct icmp packet;
				// set_udp(&packet);
				update_icmp(&packet, "", 0);
				if (sendto(data->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&(data->addr), data->addr_len) < 0) {
					// TODO: control de errores
					fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
				}
			}

			if (select(data->sockfd + 1, &read_set, NULL, NULL, &tv) < 0) {
				// TODO: el paquete no ha llegado, poner *
			}

			if (FD_ISSET(data->sockfd, &read_set)) {
				// TODO: leer el paquete
				// TODO: que no edite el sockaddr original
				ssize_t ret;
				struct sockaddr_in tmp = data->addr;
				socklen_t tmp2 = sizeof(tmp);
				ft_memset(buffer, 255, sizeof(buffer));

				if ((ret = recvfrom(data->sockfd, buffer, sizeof(buffer), 0, NULL, NULL)) < 0) { // TODO: control de errores
					fprintf(stderr, "%s:%d:\tsockfd(%d): %s\n", __FILE__, __LINE__, data->sockfd, strerror(errno)); // TODO: borrar
				}
				// TODO: sólo los que tengan el ttl expired
				// ((struct icmp*)(&buffer[sizeof(struct ip)]))->icmp_type

				printf("  %s",
					// inet_ntoa(icmp_ptr->icmp_ip.ip_src)
					inet_ntoa(ip_ptr->ip_src)
				);

				// TODO: comprobar que el paquete corresponde con los enviados (id, payload...)
				// TODO: comprobar el patete recibido para ver si ya ha terminado
			} else {
				// TODO: el else con el * de que no ha llegado en el tiempo
				printf("  *");
			}
			if (icmp_ptr->icmp_type == ICMP_ECHOREPLY)
				break; // tendria que tener los tres paquetes recibidos
		}
		printf("\n");
		destroy_connection_data(data);
		// TODO: si ya ha terminado salir del bucle
		if (icmp_ptr->icmp_type == ICMP_ECHOREPLY)
			break; // tendria que tener los tres paquetes recibidos
	}
	
}
