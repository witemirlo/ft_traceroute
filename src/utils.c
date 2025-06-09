#include <stdlib.h>

int ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0') {
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}

	return *s1 - *s2;
}

void *ft_memset(void *b, int c, size_t len)
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
