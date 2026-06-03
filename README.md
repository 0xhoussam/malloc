# malloc

A custom implementation of the C dynamic memory allocator (`malloc`, `free`, `realloc`), built as a shared library that can transparently replace the system allocator via `LD_PRELOAD`. A 42 school project.

## Overview

The allocator requests memory from the kernel in large **chunks** via `mmap`, then carves individual **blocks** out of those chunks to satisfy allocation requests. Freed blocks are tracked in a free list for reuse, and chunks are returned to the kernel with `munmap` once they hold no live allocations.

All public entry points are guarded by a single `pthread_mutex` for thread safety.

## Data structures

```c
typedef struct chunk_header_s {
  size_t size;              // total chunk size (page-aligned)
  size_t used;              // bytes used within the chunk
  size_t allocation_count;  // live allocations in this chunk
  struct chunk_header_s *next;
} chunk_header_t;

typedef struct block_header_s {
  size_t size;                  // payload size of this block
  chunk_header_t *owner_chunk;  // chunk this block belongs to
  struct block_header_s *next;  // next block in the free list
  uint64_t __padding__;
} block_header_t;
```

- `chunks` — singly linked list of all mapped chunks.
- `free_list` — singly linked list of freed blocks available for reuse.

The user pointer returned by `malloc` points just past the `block_header_t`; `free`/`realloc` recover the header with `(block_header_t *)ptr - 1`.

## Allocation strategy

`malloc(len)`:
1. Align `len` up to `ALIGNMENT` (16 bytes).
2. Try first fit from the free list.
3. Else find an existing chunk with enough room and place a new block.
4. Else `mmap` a new chunk (rounded up to page size) and place the block there.

`free(ptr)`:
- Decrement the owner chunk's `allocation_count`.
- If it reaches 0, unlink the chunk's blocks from the free list, unlink the chunk, and `munmap` it.
- Otherwise push the block onto the free list.

`realloc(ptr, size)`:
- `NULL` ptr → `malloc(size)`.
- Shrink or equal → return same pointer.
- Grow → `malloc`, `ft_memmove` the old payload, `free` the old block.

## Build

```sh
make            # builds libft_malloc.so (symlink to libft_malloc_$(HOSTTYPE).so)
make test       # builds the stress-test binary `main` linked against the lib
make clean      # remove objects and test binary
make fclean     # also remove the .so files
make re         # clean rebuild
```

`CFLAGS = -Wall -Wextra -fPIC -Og -g`

## Usage

Preload the library to override the system allocator:

```sh
LD_PRELOAD=./libft_malloc.so your_program
```

Or link against it directly (see the `test` target in the Makefile).

## Files

| File         | Purpose                                            |
|--------------|----------------------------------------------------|
| `lib.c`      | `malloc`, `free`, `realloc` entry points           |
| `utils.c`    | chunk/block management, `mmap` helpers, `ft_mem*`  |
| `malloc.h`   | types and prototypes                               |
| `main.c`     | stress test (alloc/realloc/free loop)              |
| `Makefile`   | build rules                                        |
