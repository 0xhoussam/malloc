#include "malloc.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

chunk_header_t *chunks = NULL;
block_header_t *free_list = NULL;
pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;

void *malloc(size_t len) {
  if (len == 0)
    return NULL;
  pthread_mutex_lock(&malloc_lock);
  len = align_up(len, ALIGNMENT);

  if (!chunks && (get_more_memory(len + sizeof(block_header_t))) == NULL) {
    pthread_mutex_unlock(&malloc_lock);
    return NULL;
  }

  block_header_t *free_block = get_first_fit_from_free_list(len);
  if (free_block) {
    pthread_mutex_unlock(&malloc_lock);
    return (free_block + 1);
  }

  chunk_header_t *available_chunk =
      look_for_chunk_with_available_size(len + sizeof(block_header_t));
  if (available_chunk) {
    void *ptr = place_block_in_chunk(available_chunk, len);
    pthread_mutex_unlock(&malloc_lock);
    return ptr;
  }

  chunk_header_t *test = get_more_memory(len + sizeof(block_header_t));
  if (test == NULL) {
    pthread_mutex_unlock(&malloc_lock);
    return NULL;
  }
  void *ptr =
      place_block_in_chunk(test, len); // pthread_mutex_unlock(&malloc_lock);
  pthread_mutex_unlock(&malloc_lock);
  return ptr;
}

void free(void *ptr) {
  if (!ptr)
    return;
  pthread_mutex_lock(&malloc_lock);
  block_header_t *header = (block_header_t *)ptr - 1;
  chunk_header_t *chunk = header->owner_chunk;

  chunk->allocation_count -= 1;
  if (chunk->allocation_count == 0) {
    remove_chunk_block_from_free_list(chunk);
    remove_chunk_from_chunk_list(chunk);
    munmap(chunk, chunk->size);
  } else {
    header->next = free_list;
    free_list = header;
  }
  pthread_mutex_unlock(&malloc_lock);
}

void *realloc(void *ptr, size_t size) {
  block_header_t *header;
  void *new_ptr;
  if (ptr == NULL) {
    return malloc(size);
  }

  header = (block_header_t *)ptr - 1;
  if (header->size == size)
    return ptr;
  if (header->size > size) {
    return ptr;
  }
  new_ptr = malloc(size);
  if (!new_ptr)
    return NULL;
  ft_memmove(new_ptr, ptr, header->size);
  free(ptr);
  return new_ptr;
}
