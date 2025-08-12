#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include "malloc.h"
#include "sys/mman.h"

extern chunk_header_t *chunks;
extern block_header_t *free_list;

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

static inline size_t get_actual_mmap_size(size_t requested_len) {
  int page_size = getpagesize();
  return (ceil((double)requested_len / page_size)) * page_size;
}

static inline size_t get_total_mmap_needed_len(size_t requested_len) {
    return requested_len + sizeof(block_header_t) + sizeof(chunk_header_t);
}

void *get_more_memory(size_t len) {
    size_t total_len = len + sizeof(chunk_header_t);

  chunk_header_t block_header = {
      .size = get_actual_mmap_size(total_len), .next = chunks, .used = sizeof(chunk_header_t)};

  char *chunk =
      mmap(0, total_len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (chunk == MAP_FAILED)
    return NULL;

  ft_memmove(chunk, &block_header, sizeof(block_header_t));
  chunks = (void *)chunk;
  return chunk;
}

void *get_firt_fit_from_free_list(size_t len) {
    block_header_t *tmp = free_list;

    if (free_list == NULL) return NULL;
    while (tmp) {
        if (tmp->is_free && tmp->size >= len) {
            tmp->is_free = false;
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

bool check_if_chunk_free(chunk_header_t *chunk) {
    size_t freed_size = 0;
    size_t count = 0;

    for (block_header_t *lst = free_list; lst; lst = lst->next) {
        if (lst->is_free == false) return false;
        freed_size += lst->size;
        count++;
    }
    freed_size += count * sizeof(block_header_t);
    printf("%li == %li", freed_size, chunk->size - sizeof(chunk_header_t));
    return freed_size == chunk->size - sizeof(chunk_header_t) ? true : false;
}

void unmap_pages_if_unused() {
    for (chunk_header_t *chunk = chunks; chunk; chunk = chunk->next) {
        if (check_if_chunk_free(chunk) == true) {
            printf("unmapping a page");
            munmap(chunk, chunk->size);
        }
    }
}
