#include "ft_traceroute.h"
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

uint16_t dst_port = 33434;

static void get_addrinfo(char const* const addr, struct addrinfo const* const hints, struct addrinfo** result)
{
	int             ret;
	char            buffer[BUFSIZ];

	snprintf(buffer, sizeof(buffer), "%d", dst_port);
	// ret = getaddrinfo(addr, buffer, hints, result);
	ret = getaddrinfo(addr, NULL, hints, result);
	if (ret < 0) {
		fprintf(stderr, "%s:%d:\tft_traceroute: %s: %s\n", __FILE__, __LINE__, addr, gai_strerror(ret)); // TODO: limpiar lo del comienzo
		exit(EXIT_FAILURE);
	}

	dst_port++;
}

static int get_fd_from_addrinfo(struct addrinfo* addr, struct addrinfo** rp)
{
	int sockfd = -1;

	while (addr != NULL) {
		sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sockfd > -1)
			break;
		addr = addr->ai_next;
	}

	if (addr == NULL) {
		fprintf(stderr, "%s:%d:\tft_traceroute: Error: %s\n", __FILE__, __LINE__, strerror(errno)); // TODO: limpiar lo del comienzo
		exit(EXIT_FAILURE);
	}

	*rp = addr;
	return sockfd;
}

static void set_socket_options(int sockfd)
{
	static int ttl = 1;

	if (setsockopt(sockfd, IPPROTO_IP, IP_RECVERR, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s:%d:\tft_traceroute: Error: %s\n", __FILE__, __LINE__, strerror(errno)); // TODO: limpiar lo del  comienzo
		exit(EXIT_FAILURE);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s:%d:\tft_traceroute: Error: %s\n", __FILE__, __LINE__, strerror(errno)); // TODO: limpiar lo del  comienzo
		exit(EXIT_FAILURE);
	}

	ttl++;
}

void get_connection_data(t_connection_data* data, char const* const str_addr, struct addrinfo const* const hints)
{
	struct addrinfo *result = NULL;
	struct addrinfo *rp = NULL;

	get_addrinfo(str_addr, hints, &result);
	data->sockfd = get_fd_from_addrinfo(result, &rp);

	if (data->sockfd < 0) {
		freeaddrinfo(result);
		exit(EXIT_FAILURE);
	}

	data->addr = *((struct sockaddr_in*)(rp->ai_addr));
	data->addr_len = sizeof(data->addr);

	freeaddrinfo(result);

	set_socket_options(data->sockfd);

	struct sockaddr_in tmp = {0}; // TODO: limpiar
	tmp.sin_addr.s_addr = INADDR_ANY;
	tmp.sin_family = AF_INET;
	if (bind(data->sockfd, (struct sockaddr*)&tmp, sizeof(tmp)) < 0) {
		fprintf(stderr, "ft_traceroute: Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void destroy_connection_data(t_connection_data* const data)
{
	close(data->sockfd);
}

void error_destroy_connection_data(t_connection_data* data)
{
        fprintf(stderr, "ft_traceroute: Error: %s\n", strerror(errno));
        destroy_connection_data(data);
        exit(EXIT_FAILURE);
}
