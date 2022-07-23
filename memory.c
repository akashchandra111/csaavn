#include "memory.h"
#include <stdlib.h>

memory_dyn* mem_dyn_init(size_t size)	{
	if (size > 0)	{
		memory_dyn *mem = (memory_dyn *) malloc(sizeof(memory_dyn));

		if (mem)	{
			mem->buffer = (char*) malloc(size);

			if (mem->buffer)	{
				mem->buf_sz = size;
				mem->size = 0;
				return mem;
			}
		}
	}

	return NULL;
}

void mem_dyn_free(memory_dyn *mem)	{
	if (mem)	{
		if (mem->buffer)	{
			free(mem->buffer);
			mem->buffer = NULL;
		}
		free(mem);
		mem = NULL;
	}
}
