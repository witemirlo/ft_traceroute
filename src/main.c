#include "ft_traceroute.h"

int main(int argc, char* argv[])
{
	struct addrinfo hints = {0};
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_CANONNAME;
	hints.ai_protocol = 0; 

	int ret = getaddrinfo(argv[1], NULL, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, gai_strerror(ret));
		return EXIT_FAILURE;
	}

	int sockfd;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd < 0)
			continue;
		break;
	}

	freeaddrinfo(result);
	if (sockfd < 0) {
		fprintf(stderr, "%s:%d: %s: Could not connect\n", __FILE__, __LINE__, __progname);
		return EXIT_FAILURE;
	}

	int32_t ttl = 1;
	if (setsockopt(sockfd, IPPROTO_IP, IP_RECVERR, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, strerror(errno));
		return EXIT_FAILURE;
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
