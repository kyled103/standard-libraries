#ifndef MEMLEAK_H
#define MEMLEAK_H

#define MEMLEAK_VERBOSE 0x1

#ifdef MEMLEAK_IMPLEMENTATION
#define malloc(Size) memleak_malloc(Size, __FILE__, __LINE__)
#define free(Block) memleak_free(Block)
#define calloc(Count, Size) memleak_calloc(Count, Size, __FILE__, __LINE__)
#define realloc(Block, Size) memleak_realloc(Block, Size, __FILE__, __LINE__)

void *memleak_malloc(size_t size, char *file, int line);
void memleak_free(void *ptr);
void *memleak_calloc(size_t nmemb, size_t size, char *file, int line);
void *memleak_realloc(void *ptr, size_t size, char *file, int line);

#ifndef _WIN32
#define reallocarray(x, y, z) mem_leak_reallocarray(x, y, z, __FILE__,
__LINE__) void *memleak_reallocarray(void *ptr, size_t nmemb, size_t size,
char *file,
                           int line);
#endif

#endif

int memleak_init(int flags);

#endif
