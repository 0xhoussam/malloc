#include "malloc.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

chunk_header_t *chunks = NULL;

void *malloc(size_t len) {
  int page_size;
  chunk_header_t page_header;
  block_header_t block_header;

  page_size = getpagesize();
  if (chunks == NULL) {
    void *memory =
        mmap(0, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (memory == MAP_FAILED)
      return NULL;

    page_header.size = page_size;
    page_header.used =
        sizeof(chunk_header_t) + sizeof(block_header_t) + len;
    page_header.next = NULL;
    ft_memmove(memory, &page_header, sizeof(page_header));
    chunks = memory;

    block_header.is_free = false;
    block_header.size = len;
    block_header.next = NULL;
    ft_memmove((chunk_header_t *)memory + 1, &block_header, sizeof(block_header_t));
    return (block_header_t *)((chunk_header_t *)memory + 1) + 1;
  }
  return mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0,
              0);
}
