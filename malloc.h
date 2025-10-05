#ifndef MALLOC_H
#define MALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>

typedef struct chunk_header_s {
  size_t size;
  size_t used;
  size_t allocation_count;
  struct chunk_header_s *next;
} chunk_header_t;

typedef struct block_header_s {
  size_t size;
  bool is_free;
  chunk_header_t *owner_chunk;
  struct block_header_s *next;
} block_header_t;

void *malloc(size_t len);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

// libft

void *ft_memcpy(void *dst, const void *src, size_t n);
void *ft_memmove(void *dst, const void *src, size_t len);

// utils

void *get_more_memory(size_t len);
void *get_first_fit_from_free_list(size_t len);
size_t align_up(size_t size, size_t alignment);
void split_block_if_needed(block_header_t *block, size_t size);
void remove_chunk_from_chunk_list(chunk_header_t *chunk);
chunk_header_t *look_for_chunk_with_available_size(size_t size);
void *place_block_in_chunk(chunk_header_t *chunk, size_t len);
void remove_chunk_block_from_free_list(chunk_header_t *chunk);

#endif // !MALLOC_H
