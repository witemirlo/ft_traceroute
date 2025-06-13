#include "ft_traceroute.h"

static void get_addrinfo(char const* const addr, struct addrinfo const* const hints, struct addrinfo** result)
{
	static uint32_t port = 33434;
	int             ret;
	char            buffer[BUFSIZ];

	snprintf(buffer, sizeof(buffer), "%d", port);
	ret = getaddrinfo(addr, buffer, hints, result);
	if (ret < 0) {
		fprintf(stderr, "%s: %s: %s\n", __progname, addr, gai_strerror(ret));
		exit(EXIT_FAILURE);
	}

	port++;
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
		fprintf(stderr, "%s: Error: %s\n", __progname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	*rp = addr;
	return sockfd;
}

static void set_socket_options(int sockfd)
{
	static int ttl = 1;

	if (setsockopt(sockfd, IPPROTO_IP, IP_RECVERR, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s: Error: %s\n", __progname, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s: Error: %s\n", __progname, strerror(errno));
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
}

void destroy_connection_data(t_connection_data* const data)
{
	close(data->sockfd);
}

void error_destroy_connection_data(t_connection_data* data)
{
        fprintf(stderr, "%s: Error: %s\n", __progname, strerror(errno));
        destroy_connection_data(data);
        exit(EXIT_FAILURE);
}
