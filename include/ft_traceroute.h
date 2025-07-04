#ifndef _FT_TRACEROUTE_H
# define _FT_TRACEROUTE_H

# include <arpa/inet.h>
# include <errno.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>

// - PROGRAM NAME --------------------------------------------------------------
extern uint16_t dst_port;

// - TYPES ---------------------------------------------------------------------
typedef struct s_connection_data {
        struct sockaddr_in addr;
        socklen_t          addr_len;
        int                sockfd;
} t_connection_data;

typedef struct s_packet
{
        struct udphdr udphdr;
        char          payload[32];
} t_packet;

// - INIT ----------------------------------------------------------------------
char const* init(int argc, char *argv[]);

// - ROUTINE -------------------------------------------------------------------
void routine(t_connection_data* const data, char const* const addr);

// - CONNECTION DATA -----------------------------------------------------------
void get_connection_data(t_connection_data* data, char const* const str_addr, struct addrinfo const* const hints);

void destroy_connection_data(t_connection_data* const data);
void error_destroy_connection_data(t_connection_data* data);

// - UTILS ---------------------------------------------------------------------
int ft_strcmp(const char *s1, const char *s2);
void *ft_memset(void *b, int c, size_t len);
void *ft_memcpy(void *dst, const void *src, size_t n);

// - UDP PACKETS ---------------------------------------------------------------
void set_udp(t_packet* const packet);
uint16_t udp_checksum(t_packet const* const packet);

// - ICMP PACKETS --------------------------------------------------------------
void update_icmp(struct icmp* const icmp, void const* const payload, size_t payload_size);
void init_icmp(struct icmp* const icmp);

#endif
