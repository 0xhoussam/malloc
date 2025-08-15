#ifndef MALLOC_H
#define MALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>
// #include <pthread.h>

typedef struct chunk_header_s {
  size_t size;
  size_t used;
  struct chunk_header_s *next;
} chunk_header_t;

typedef struct block_header_s {
  size_t size;
  bool is_free;
  struct block_header_s *next;
} block_header_t;

void *malloc(size_t len);
void free(void *ptr);

// libft

void *ft_memcpy(void *dst, const void *src, size_t n);
void *ft_memmove(void *dst, const void *src, size_t len);

// utils

void *get_more_memory(size_t len);
void *get_first_fit_from_free_list(size_t len);
void unmap_pages_if_unused();
size_t align_up(size_t size, size_t alignment);
void split_block_if_needed(block_header_t *block, size_t size);

#endif // !MALLOC_H
