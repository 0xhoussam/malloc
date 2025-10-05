#include "malloc.h"
#include "sys/mman.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
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

// static inline size_t get_total_mmap_needed_len(size_t requested_len) {
//     return requested_len + sizeof(block_header_t) + sizeof(chunk_header_t);
// }

void *get_more_memory(size_t len) {
  size_t total_len = len + sizeof(chunk_header_t);
  size_t actual_len = get_actual_mmap_size(total_len);

  chunk_header_t block_header = {.size = actual_len,
                                 .next = chunks,
                                 .used = sizeof(chunk_header_t),
                                 .allocation_count = 0};

  char *chunk = mmap(0, actual_len, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (chunk == MAP_FAILED)
    return NULL;

  ft_memmove(chunk, &block_header, sizeof(block_header_t));
  chunks = (void *)chunk;
  return chunk;
}

void *get_first_fit_from_free_list(size_t len) {
  block_header_t *tmp = free_list;

  if (free_list == NULL)
    return NULL;
  while (tmp) {
    if (tmp->is_free && tmp->size >= len) {
      tmp->is_free = false;
      split_block_if_needed(tmp, len);
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}

size_t align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

void split_block_if_needed(block_header_t *block, size_t size) {
  if (block->size <= size + sizeof(block_header_t))
    return;

  block_header_t b = {
      .is_free = true,
      .size = block->size - size - sizeof(block_header_t),
      .next = free_list,
      .owner_chunk = block->owner_chunk
  };
  ft_memmove((char *)block + size + sizeof(block_header_t), &b, sizeof(b));
  block->size -= size + sizeof(block_header_t);
  return;
}

void remove_chunk_from_chunk_list(chunk_header_t *chunk) {
  chunk_header_t *curr, *prev = NULL;

  if (!chunk || !chunks)
    return;
  if (chunk == chunks) {
    chunks = chunks->next;
    return;
  }
  curr = chunks;
  while (curr) {
    if (curr == chunk) {
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

    if (!chunk) return;
    while (curr) {
        if (curr->owner_chunk != chunk) {
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
    if (chunk->size - chunk->used >= size)
      return chunk;
  }
  return NULL;
}

void *place_block_in_chunk(chunk_header_t *chunk, size_t len) {
  block_header_t block = {
      .owner_chunk = chunk,
      .is_free = false,
      .size = len,
      .next = NULL,
  };
  char *available_space = (char *)chunk + chunk->used;
  chunk->used += len + sizeof(block_header_t);
  chunk->allocation_count += 1;
  ft_memmove(available_space, &block, sizeof(block));

  return (block_header_t *)available_space + 1;
}
