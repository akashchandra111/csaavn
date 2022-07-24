#ifndef SAAVN_MEMORY_H
#define SAAVN_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define BUF_SZ (3 * 1024 * 1024)

typedef struct	{
	char buffer[BUF_SZ];
	size_t size;
} memory_fixed;

typedef struct	{
	char *buffer;
	size_t buf_sz;
	size_t size;
} memory_dyn;

memory_dyn* mem_dyn_init(size_t);
void mem_dyn_free(memory_dyn*);

#endif
