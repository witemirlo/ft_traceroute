#include "ft_traceroute.h"

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

void set_udp(t_packet* const packet)
{
	static const uint8_t payload[] = "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f";
	static uint16_t      dest_port = 33434;

        ft_memset(packet, 0, sizeof(packet->udphdr));
        ft_memcpy(packet->payload, payload, sizeof(packet->payload));

	packet->udphdr.source = htons(0); // TODO: PONER LOS DATOS
	packet->udphdr.dest = htons(dest_port); // TODO: PONER LOS DATOS
	packet->udphdr.len = htons(sizeof(*packet)); // TODO: PONER LOS DATOS
        packet->udphdr.check = udp_checksum(packet);
	
	dest_port++;
}
