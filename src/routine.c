#include "ft_traceroute.h"

const uint8_t max_hops = 30;
char          msg[BUFSIZ];

static void dump(void const* const buffer, size_t size)
{
	printf("\n");
	for (size_t i = 0; i < size; i++)
		printf("%x ", ((uint8_t*)buffer)[i]);
	printf("\n");
}

static double get_time_diff(void const* const packet, t_connection_data* data)
{
	struct icmp const* icmp = (struct icmp*)((uint8_t*)packet + 8 + 20);
	struct timeval tv;
	long double t1, t2;

	if (gettimeofday(&tv, NULL) < 0)
		error_destroy_connection_data(data);

	t1 = ((uint32_t)tv.tv_sec * 1000) + ((uint32_t)tv.tv_usec / 1000.);
	t2 = (ntohl(icmp->icmp_otime) * 1000) + (ntohl(icmp->icmp_rtime) / 1000.);

	return t1 - t2;
}

static bool routine_send(t_connection_data* data)
{
	struct icmp packet;
	fd_set      write_set;

	init_icmp(&packet);

	FD_ZERO(&write_set);
	FD_SET(data->sockfd, &write_set);

	if (select(data->sockfd + 1, NULL, &write_set, NULL, NULL) < 0) {
		return false;
	}

	if (FD_ISSET(data->sockfd, &write_set)) {
		update_icmp(&packet, "", 0);
		if (sendto(data->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&(data->addr), data->addr_len) < 0) {
			// TODO: control de errores
			fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
		}
	}

	return true;
}

static int routine_receive(t_connection_data* data)
{
	struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
	fd_set         read_set;

	unsigned char  buffer[BUFSIZ];
	struct ip*     ip_ptr = (struct ip*)buffer;
	struct icmp*   icmp_ptr = (struct icmp*)(buffer + sizeof(struct ip));

	ft_memset(buffer, 42, sizeof(buffer));

	FD_ZERO(&read_set);
	FD_SET(data->sockfd, &read_set);

	if (select(data->sockfd + 1, &read_set, NULL, NULL, &tv) < 0)
		error_destroy_connection_data(data);

	if (FD_ISSET(data->sockfd, &read_set)) {
		ssize_t ret;

		if ((ret = recvfrom(data->sockfd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr*)&data->addr, &data->addr_len)) < 0)
			error_destroy_connection_data(data);

		if (icmp_ptr->icmp_type == ICMP_ECHO)
			return routine_receive(data);

		
		double time = get_time_diff(icmp_ptr, data);

		snprintf(msg, sizeof(msg), "%s TODO[%.2f] ", inet_ntoa(ip_ptr->ip_src), time);
		write(STDIN_FILENO, msg, ft_strlen(msg));
	}
	else {
		snprintf(msg, sizeof(msg), "* ");
		write(STDIN_FILENO, msg, ft_strlen(msg));
	}

	if (icmp_ptr->icmp_type == ICMP_ECHOREPLY)
		return ICMP_ECHOREPLY;
	return ICMP_ECHO;
}

void routine(t_connection_data* const data, char const* const addr)
{
	const uint8_t  packets_per_round = 3;
	uint8_t        packets_arrived;

	for (uint8_t ttl_round = 1; ttl_round <= max_hops; ttl_round++) {
		snprintf(msg, sizeof(msg), "%2d ", ttl_round);
		write(STDIN_FILENO, msg, ft_strlen(msg));

		get_connection_data(data, addr);

		packets_arrived = 0;
		for (int8_t n_packet = 0; n_packet < packets_per_round; n_packet++) { // TODO: cambiar a 3
			routine_send(data);

			if (routine_receive(data) == ICMP_ECHOREPLY)
				packets_arrived++;
		}

		snprintf(msg, sizeof(msg), "\n");
		write(STDIN_FILENO, msg, ft_strlen(msg));

		destroy_connection_data(data);

		if (packets_arrived == packets_per_round)
			break;
	}
	destroy_connection_data(data);
}
