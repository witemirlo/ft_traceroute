#include "ft_traceroute.h"
#include <netinet/in.h>

static u_int16_t sum_ones_complement(u_int16_t a, u_int16_t b)
{
        u_int32_t c = (u_int32_t)a + (u_int32_t)b;

        while(0xff0000 & c)
                c = (c & 0xffff) + 1;

        return c;
}

uint16_t udp_checksum(t_packet const* const packet)
{
        uint16_t checksum;

        checksum = 0;

        // checksum = sum_ones_complement(packet->udphdr.source, packet->udphdr.dest);
        // checksum = sum_ones_complement(checksum, packet->udphdr.len);

	// TODO: ver si podria ser con 16 bits directemente
        for (size_t i = 0; i < sizeof(*packet); i++) {
                if (i % 2 == 0)
                        checksum = sum_ones_complement(checksum, ((uint8_t*)packet)[i]);
                else
                        checksum = sum_ones_complement(checksum, ((uint8_t*)packet)[i] << 8);
        }

        checksum = 0xffff - checksum;

        return checksum;
}

void update_udp(t_packet* const packet)
{
        packet->udphdr.check = udp_checksum(packet);
}

void init_udp(t_packet* const packet)
{
        memset(packet, 0, sizeof(*packet));

	packet->udphdr.source = htons(0); // TODO: PONER LOS DATOS
	packet->udphdr.dest = htons(0); // TODO: PONER LOS DATOS
	packet->udphdr.len = htons(sizeof(*packet)); // TODO: PONER LOS DATOS
	packet->udphdr.check = udp_checksum(packet);
}
