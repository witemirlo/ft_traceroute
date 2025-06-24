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
        uint16_t checksum = 0;

        for (size_t i = 0; i < sizeof(*packet) / 2; i++)
                checksum = sum_ones_complement(checksum, ((uint16_t*)packet)[i]);

        checksum = 0xffff - checksum;

        return checksum;
}

void set_udp(t_packet* const packet)
{
	static const uint8_t payload[] = "404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f";
	const uint16_t       src_port = dst_port + payload[dst_port % sizeof(payload)];

        ft_memset(packet, 0, sizeof(packet->udphdr));
        ft_memcpy(packet->payload, payload, sizeof(packet->payload));

	packet->udphdr.source = htons(src_port); // TODO: PONER LOS DATOS
	packet->udphdr.dest = htons(dst_port); // TODO: PONER LOS DATOS
	packet->udphdr.len = htons(sizeof(*packet)); // TODO: PONER LOS DATOS
        packet->udphdr.check = udp_checksum(packet);
}
