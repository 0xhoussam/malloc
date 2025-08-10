#include "malloc.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

chunk_header_t *chunks = NULL;
block_header_t *free_list = NULL;

void *malloc(size_t len) {
  block_header_t block_header = {
      .size = len,
      .is_free= false,
      .next = NULL,
  };

  if (!chunks && (get_more_memory(len + sizeof(block_header_t))) == NULL) {
    return NULL;
  }

  block_header_t *free_block = get_firt_fit_from_free_list(len);
  if (free_block)
    return (free_block + 1);
  if (chunks->size - chunks->used < len + sizeof(block_header) && (get_more_memory(len)) == MAP_FAILED) {
    return NULL;
  }
  char *available_space = (char *)chunks + chunks->used;
  chunks->used += len + sizeof( block_header_t);
  ft_memmove(available_space, &block_header, sizeof(block_header_t));
  return ((block_header_t *)available_space) + 1;
}

void free(void *ptr) {
    block_header_t *header = (block_header_t *)ptr - 1;
    header->is_free = true;
    header->next = free_list;
    free_list = header;
}

int main() {
    char *ptr = malloc(12);
    ft_memmove(ptr, "hello world\n", sizeof("hello world\n"));
    printf("%s\n", ptr);
    free(ptr);
}
