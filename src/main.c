#include "ft_traceroute.h"

int main(int argc, char* argv[])
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, strerror(errno));
		return EXIT_FAILURE;
	}

	int ip = (72 << 24) | (14 << 16) | (219 << 8) | (20);
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = ip,
		.sin_port = 8080,
		.sin_zero = 0
	};

	int32_t ttl = 2;
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, strerror(errno));
		return EXIT_FAILURE;
	}

	if (sendto(sockfd, "hi", 2, 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0) {
		fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, __progname, strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
