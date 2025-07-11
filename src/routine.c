#include "ft_traceroute.h"

const uint8_t max_hops = 30;
const uint8_t packets_per_round = 3;
char          msg[BUFSIZ];

static void
dump(void const* const buffer, size_t size)
{
	printf("\n");
	for (size_t i = 0; i < size; i++)
		printf("%x ", ((uint8_t*)buffer)[i]);
	printf("\n");
}

static double
calculate_time(t_connection_data* data, struct timeval const* start_tv)
{
	struct timeval end_tv;

	if (gettimeofday(&end_tv, NULL) < 0)
		error_destroy_connection_data(data);

	return (
		((end_tv.tv_sec * 1000) + (end_tv.tv_usec / 1000.))
		- ((start_tv->tv_sec * 1000) + (start_tv->tv_usec / 1000.))
	);
}

static void
print(t_connection_data* data, struct timeval const* start_tv, struct ip* ip, bool reset)
{
	static in_addr_t last_addr = 0;
	static bool      gateway = true;
	const char       *str = NULL;	
	double           time;

	if (reset) {
		last_addr = 0;
		gateway = false;
		snprintf(msg, sizeof(msg), "\n");
		write(STDIN_FILENO, msg, ft_strlen(msg));
		return;
	}

	time = calculate_time(data, start_tv);

	if (ip->ip_src.s_addr != last_addr) {
		last_addr = ip->ip_src.s_addr;
		str = inet_ntoa(ip->ip_src);

		snprintf(msg, sizeof(msg), "%s (%s) ", (gateway) ? "_gateway" : str, str);
		write(STDIN_FILENO, msg, ft_strlen(msg));
	}

	snprintf(msg, sizeof(msg), " %.3fms ", time);
	write(STDIN_FILENO, msg, ft_strlen(msg));
}

static bool
routine_send(t_connection_data* data)
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
			fprintf(stderr, "%s:%d:\t%s\n", __FILE__, __LINE__, strerror(errno));
			error_destroy_connection_data(data);
		}
	}

	return true;
}

static int
routine_receive(t_connection_data* data, struct timeval const* start_tv)
{
	struct timeval tv = {.tv_sec = 3, .tv_usec = 0};
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
			return routine_receive(data, start_tv);

		print(data, start_tv, ip_ptr, false);
	}
	else {
		snprintf(msg, sizeof(msg), "* ");
		write(STDIN_FILENO, msg, ft_strlen(msg));
	}

	if (icmp_ptr->icmp_type == ICMP_ECHOREPLY)
		return ICMP_ECHOREPLY;
	return ICMP_ECHO;
}

void
routine(t_connection_data* const data, char const* const addr)
{
	uint8_t        packets_arrived;
	struct timeval start_tv;

	for (uint8_t ttl_round = 1; ttl_round <= max_hops; ttl_round++) {
		snprintf(msg, sizeof(msg), "%2d  ", ttl_round);
		write(STDIN_FILENO, msg, ft_strlen(msg));

		get_connection_data(data, addr);

		packets_arrived = 0;
		for (int8_t n_packet = 0; n_packet < packets_per_round; n_packet++) {
			routine_send(data);

			if (gettimeofday(&start_tv, NULL) < 0)
				error_destroy_connection_data(data);
			if (routine_receive(data, &start_tv) == ICMP_ECHOREPLY)
				packets_arrived++;
		}

		print(data, NULL, NULL, true);

		destroy_connection_data(data);

		if (packets_arrived == packets_per_round)
			break;
	}
}
