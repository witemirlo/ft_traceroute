#include "ft_traceroute.h"

static u_int16_t
sum_ones_complement(u_int16_t a, u_int16_t b)
{
	u_int32_t c = (u_int32_t)a + (u_int32_t)b;

	while(0xff0000 & c)
		c = (c & 0xffff) + 1;

	return c;
}

static void
icmp_timestamp(struct icmp* const icmp)
{
	static struct timeval tv = {0};

	if (gettimeofday(&tv, NULL) < 0)
		return;

	icmp->icmp_otime = htonl(tv.tv_sec);
	icmp->icmp_rtime = htonl(tv.tv_usec);
}

void
icmp_checksum(struct icmp* const icmp, void const* const payload, size_t payload_size)
{
	icmp->icmp_cksum = 0;

	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_type, icmp->icmp_code);
	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, icmp->icmp_id);
	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, icmp->icmp_seq);

	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, (icmp->icmp_otime >> 16));
	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, (icmp->icmp_otime & 0xffff));

	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, (icmp->icmp_rtime >> 16));
	icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, (icmp->icmp_rtime & 0xffff));

	for (size_t i = 0; i < payload_size; i++) {
		if (i % 2 == 0)
			icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, ((uint8_t*)payload)[i]);
		else
			icmp->icmp_cksum = sum_ones_complement(icmp->icmp_cksum, ((uint8_t*)payload)[i] << 8);
	}

	icmp->icmp_cksum = 0xffff - icmp->icmp_cksum;
}

void
update_icmp(struct icmp* const icmp, void const* const payload, size_t payload_size)
{
	static uint16_t seq = 0;

	icmp_timestamp(icmp);
	icmp->icmp_seq = htons(seq++);
	icmp_checksum(icmp, payload, payload_size);
}

void
init_icmp(struct icmp* const icmp)
{
	ft_memset(icmp, 0, sizeof(*icmp));

	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = 0x42;
}
