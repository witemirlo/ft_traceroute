#include "ft_traceroute.h"

bool dns_resolution = false;
uint8_t current_hop = 1;
uint8_t max_hop = 30;
uint8_t packets_per_round = 3;

static void
usage_and_exit(FILE* stream, int code)
{
	fprintf(stream,
		"Usage:\n"
		"  ft_traceroute host\n"
		"Options:\n"
		"  --first=first_ttl  Start from the first_ttl hop (instead from 1)\n"
		"  --help             Read this help and exit\n"
		"  --max-hops=max_ttl Set the max number of hops (max TTL to be reached). Default is 30\n"
		"  --queries=nqueries Set the number of probes per each hop. Default is 3\n"
		"  --resolve-dns      Make dns resolution\n"
		"\n"
		"Arguments:\n"
		"+     host          The host to traceroute to\n"
	);

	exit(code);
}

static int
parse_num(char const* const str)
{
	if (*str == '\0')
		return -1;

        for (size_t i = 0; str[i]; i++) {
                if (i < 5 && ft_isdigit(str[i]))
                        continue;

                fprintf(stderr, "ft_traceroute: invalid value (`%s' near `%s')\n", str, str + i);
                return -1;
        }

        return ft_atoi(str);
}

static void
case_first(char const* const str)
{
	int n;

	if ((n = parse_num(str)) < 0)
	{
		fprintf(stderr, "Cannot handle `--first' option with arg `%s'\n", str);
		exit(EXIT_FAILURE);
	}

	if (n > 255)
	{
		fprintf(stderr, "first hop out of range\n");
		exit(EXIT_FAILURE);
	}
	
	if (n < 1)
	{
		fprintf(stderr, "first hop out of range\n");
		exit(EXIT_FAILURE);
	}
	
	current_hop = n;
}

static void
case_max(char const* const str)
{
	int n;

	if ((n = parse_num(str)) < 0)
	{
		fprintf(stderr, "Cannot handle `--max-hops' option with arg `%s'\n", str);
		exit(EXIT_FAILURE);
	}

	if (n > 255)
	{
		fprintf(stderr, "max hops cannot be more than 255\n");
		exit(EXIT_FAILURE);
	}

	if (n < 1)
	{
		fprintf(stderr, "max hops out of range\n");
		exit(EXIT_FAILURE);
	}

	max_hop = n;
}

static void
case_resolve_dns(void)
{
	dns_resolution = true;
}

static void
case_queries(char const* const str)
{
	int n;

	if ((n = parse_num(str)) < 0)
	{
		fprintf(stderr, "Cannot handle `--queries' option with arg `%s'\n", str);
		exit(EXIT_FAILURE);
	}

	if (n > 10)
	{
		fprintf(stderr, "no more than 10 probes per hop\n");
		exit(EXIT_FAILURE);
	}
	
	if (n < 1)
	{
		fprintf(stderr, "no less than 1 probes per hop\n");
		exit(EXIT_FAILURE);
	}

	packets_per_round = n;
}

static void
option(char const* const str)
{
	if (ft_strncmp(str, "first=", ft_strlen("first=")) == 0)
		return case_first(str + ft_strlen("first="));

	if (ft_strncmp(str, "max-hops=", ft_strlen("max-hops=")) == 0)
		return case_max(str + ft_strlen("max-hops="));

	if (ft_strncmp(str, "resolve-dns", ft_strlen("resolve-dns")) == 0)
		return case_resolve_dns();

	if (ft_strncmp(str, "queries=", ft_strlen("queries=")) == 0)
		return case_queries(str + ft_strlen("queries="));

	fprintf(stderr, "Bad option `--%s'\n", str);
	exit(EXIT_FAILURE);
}

void
check_first_hop_is_lower_than_max_hop()
{
	if (current_hop > max_hop)
	{
		fprintf(stderr, "first hop out of range\n");
		exit(EXIT_FAILURE);
	}
}

char const*
init(int argc, char *argv[])
{
	char const* addr = NULL;

	for (int i = 1; i < argc; i++) {
		if (ft_strcmp(argv[i], "--help") == 0 ||
		    addr != NULL)
			usage_and_exit(stderr, EXIT_SUCCESS);

		if (ft_strncmp(argv[i], "--", ft_strlen("--")) == 0)
		{
			option(argv[i] + 2);
			continue;
		}

		if (addr == NULL)
			addr = argv[i];
	}

	if (addr == NULL)
		usage_and_exit(stdout, EXIT_FAILURE);

	check_first_hop_is_lower_than_max_hop();

	return addr;
}
