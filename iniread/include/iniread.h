#ifndef INIREAD_H
#define INIREAD_H

#include <stdint.h>

// Change Log
// Version  | DATE      | UPDATE
// 1.0        11/18/26   Initial version

// Flags
#define INIREAD_VERBOSE 0x1

// Error Codes
#define INIREAD_ERROR_SUCCESS 0x0
#define INIREAD_ERROR_NO_MEM 0x1
#define INIREAD_ERROR_NULL_INPUT 0x2
#define INIREAD_ERROR_FILE_ERROR 0x3
#define INIREAD_ERROR_ARRAY 0x4

typedef struct iniread_s *INIREAD;

INIREAD iniread_create(char *file, uint32_t flags);
uint32_t iniread_destroy(INIREAD *ini);

uint32_t iniread_read_int(INIREAD ini, char *section, char *token, int *output);
uint32_t iniread_read_dbl(INIREAD ini, char *section, char *token,
                          double *output);
uint32_t iniread_read_char(INIREAD ini, char *section, char *token,
                           char **output);

#endif
