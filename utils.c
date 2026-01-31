#include "malloc.h"
#include "sys/mman.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern chunk_header_t *chunks;
extern block_header_t *free_list;

void *ft_memcpy(void *dst, const void *src, size_t n) {
  unsigned char *c_dst;
  const unsigned char *c_src;

  if (!dst && !src) {
    return (NULL);
  }
  c_dst = (unsigned char *)dst;
  c_src = (const unsigned char *)src;
  while (n > 0) {
    *c_dst = *c_src;
    n--;
    c_src++;
    c_dst++;
  }
  return (dst);
}

void *ft_memmove(void *dst, const void *src, size_t len) {
  unsigned char *u_dst;
  const unsigned char *u_src;

  if (!dst && !src)
    return (NULL);
  u_dst = (unsigned char *)dst;
  u_src = (const unsigned char *)src;
  if (dst < src) {
    ft_memcpy(dst, src, len);
  } else {
    while (len > 0) {
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

void *get_more_memory(size_t len) {
  size_t total_len = len + sizeof(chunk_header_t);
  size_t actual_len = get_actual_mmap_size(total_len);

  chunk_header_t chunk_header = {.size = actual_len,
                                 .next = chunks,
                                 .used = sizeof(chunk_header_t),
                                 .allocation_count = 0};

  char *chunk = mmap(NULL, actual_len, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (chunk == MAP_FAILED)
    return NULL;

  ft_memmove(chunk, &chunk_header, sizeof(chunk_header_t));
  chunks = (void *)chunk;
  return chunk;
}

void *get_first_fit_from_free_list(size_t len) {
  block_header_t *curr = free_list, *prev = NULL;

  if (free_list == NULL)
    return NULL;
  while (curr) {
    if (curr->size >= len) {
      // split_block_if_needed(curr, len);
      curr->owner_chunk->allocation_count += 1;
      if (prev == NULL) {
        free_list = free_list->next;
        return curr;
      }
      prev->next = curr->next;
      return curr;
    }
    prev = curr;
    curr = curr->next;
  }
  return NULL;
}

size_t align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

// void split_block_if_needed(block_header_t *block, size_t size) {
//   if (block->size < size + sizeof(block_header_t) + ALIGNMENT)
//     return;
//
//   block_header_t *new_block =
//       (block_header_t *)((char *)block + size + sizeof(block_header_t));
//   new_block->size = block->size - size - sizeof(block_header_t);
//   new_block->next = free_list;
//   new_block->owner_chunk = block->owner_chunk;
//   free_list = new_block;
//   block->size = size;
//   return;
// }

void remove_chunk_from_chunk_list(chunk_header_t *chunk) {
  chunk_header_t *curr, *prev = NULL;

  if (!chunk || !chunks)
    return;
  curr = chunks;
  while (curr) {
    if (curr == chunk) {
      if (prev == NULL) {
        chunks = chunks->next;
        break;
      }
      prev->next = curr->next;
      break;
    };
    prev = curr;
    curr = curr->next;
  }
  return;
}

void remove_chunk_block_from_free_list(chunk_header_t *chunk) {
  block_header_t *prev = NULL, *curr = free_list;

  if (!chunk)
    return;
  while (curr) {
    if (curr->owner_chunk != chunk) {
      prev = curr;
      curr = curr->next;
      continue;
    };
    if (prev == NULL) {
      free_list = curr->next;
      curr = free_list;
      continue;
    }
    prev->next = curr->next;
    curr = curr->next;
  }
}

chunk_header_t *look_for_chunk_with_available_size(size_t size) {
  for (chunk_header_t *chunk = chunks; chunk; chunk = chunk->next) {
    if (chunk->size >= chunk->used + size)
      return chunk;
  }
  return NULL;
}

void *place_block_in_chunk(chunk_header_t *chunk, size_t len) {

  block_header_t *available_space =
      (block_header_t *)((char *)chunk + chunk->used);
  available_space->size = len;
  available_space->owner_chunk = chunk;
  available_space->next = NULL;

  chunk->used += len + sizeof(block_header_t);
  chunk->allocation_count += 1;

  return (block_header_t *)available_space + 1;
}
