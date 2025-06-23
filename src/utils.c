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

void    *ft_memcpy(void *dst, const void *src, size_t n)
{
        unsigned char           *ptrdst;
        const unsigned char     *ptrsrc;
        size_t                          i;

        if (dst == NULL && src == NULL)
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
