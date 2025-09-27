#include "ft_traceroute.h"

void
print_header(t_connection_data* const data, char const* const addr)
{
	snprintf(msg, sizeof(msg), "ft_traceroute to %s (%s), %d hops max, 48 byte packets\n"
		,addr
		,inet_ntoa(data->addr.sin_addr)
		,max_hops);

	write(STDOUT_FILENO, msg, ft_strlen(msg));
}

int
ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0') {
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}

	return *s1 - *s2;
}

void*
ft_memset(void *b, int c, size_t len)
{
	size_t         i;
	unsigned char *ptrb;

	ptrb = b;
	i = 0;
	while (i < len)
	{
		ptrb[i] = c;
		i++;
	}
	return (ptrb);
}

void*
ft_memcpy(void *dst, const void *src, size_t n)
{
        unsigned char           *ptrdst;
        const unsigned char     *ptrsrc;
        size_t                          i;

        if (dst == NULL || src == NULL)
                return (NULL);
        ptrdst = dst;
        ptrsrc = src;
        i = 0;
        while (i < n)
        {
                ptrdst[i] = ptrsrc[i];
                i++;
        }
        return (ptrdst);
}

size_t
ft_strlen(const char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = 0;
	while (s[len])
		len++;
	return (len);
}

int
ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}

int
ft_atoi(const char *str)
{
	char *ptr;
	int  total;
	int  sign;

	ptr = (char *) str;
	total = 0;
	sign = 1;
	while (*ptr == ' ' || *ptr == '\t' || *ptr == '\v'
		|| *ptr == '\r' || *ptr == '\n' || *ptr == '\f')
		ptr++;
	if (*ptr == '-' || *ptr == '+')
	{
		if (*ptr == '-')
			sign *= -1;
		ptr++;
	}
	while (*ptr >= '0' && *ptr <= '9')
	{
		total = total * 10 + (*ptr - '0');
		ptr++;
	}
	return (total * sign);
}

int
ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if (n == 0 || s1 == NULL || s2 == NULL)
		return (0);
	i = 0;
	while (i < n && s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return ((unsigned char) s1[i] - (unsigned char) s2[i]);
		i++;
	}
	if (i == n)
		return (s1[i - 1] - s2[i - 1]);
	return ((unsigned char) s1[i] - (unsigned char) s2[i]);
}
