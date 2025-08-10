#include <stddef.h>

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*c_dst;
	const unsigned char	*c_src;

	if (!dst && !src)
	{
		return (NULL);
	}
	c_dst = (unsigned char *)dst;
	c_src = (const unsigned char	*)src;
	while (n > 0)
	{
		*c_dst = *c_src;
		n--;
		c_src++;
		c_dst++;
	}
	return (dst);
}

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char		*u_dst;
	const unsigned char	*u_src;

	if (!dst && !src)
		return (NULL);
	u_dst = (unsigned char *)dst;
	u_src = (const unsigned char *)src;
	if (dst < src)
	{
		ft_memcpy(dst, src, len);
	}
	else
	{
		while (len > 0)
		{
			u_dst[len - 1] = u_src[len - 1];
			len--;
		}
	}
	return (dst);
}

