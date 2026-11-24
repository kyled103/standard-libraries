#define memdump(addr, size, width)                                             \
  do {                                                                         \
    char *bytes = addr;                                                        \
    unsigned int cursor = 0;                                                   \
    unsigned int addr_size = sizeof(void *);                                   \
    printf("Memory Dump from %p in %s:%d\n", addr, __FILE__, __LINE__);        \
    printf("\u250C");                                                          \
    for (int i = 0; i < addr_size * 2; i++)                                    \
      printf("\u2500");                                                        \
    printf("\u252C");                                                          \
    for (int i = 0; i < (3 * width) - 1; i++)                                  \
      printf("\u2500");                                                        \
    printf("\u252C");                                                          \
    for (int i = 0; i < (2 * width) - 1; i++)                                  \
      printf("\u2500");                                                        \
    printf("\u2510\n");                                                        \
    for (int i = 0; i < size; i += width) {                                    \
      printf("\u2502");                                                        \
      printf("0x%p", &bytes[cursor]);                                          \
      printf("\u2502");                                                        \
      for (int i = 0; i < width; i++) {                                        \
        if (cursor >= size)                                                    \
          printf("\x1B[90m");                                                  \
        printf("%02X ", (unsigned char)bytes[cursor]);                         \
        cursor++;                                                              \
      }                                                                        \
      printf("\x1B[39m%c\u2502", 0x8);                                         \
      cursor -= width;                                                         \
      for (int i = 0; i < width; i++) {                                        \
        if (cursor >= size)                                                    \
          printf("\x1B[90m");                                                  \
                                                                               \
        char c = (char)bytes[cursor];                                          \
        if (c < 0x20 || c >= 0x7F)                                             \
          printf("? ");                                                        \
        else                                                                   \
          printf("%c ", (char)bytes[cursor]);                                  \
        cursor++;                                                              \
      }                                                                        \
      printf("\x1B[39m%c\u2502\n", 0x8);                                       \
    }                                                                          \
    printf("\u2514");                                                          \
    for (int i = 0; i < addr_size * 2; i++)                                    \
      printf("\u2500");                                                        \
    printf("\u2534");                                                          \
    for (int i = 0; i < (3 * width) - 1; i++)                                  \
      printf("\u2500");                                                        \
    printf("\u2534");                                                          \
    for (int i = 0; i < (2 * width) - 1; i++)                                  \
      printf("\u2500");                                                        \
    printf("\u2518\n");                                                        \
  } while (0)
