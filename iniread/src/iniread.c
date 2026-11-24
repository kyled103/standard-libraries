#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../build/include/darray.h"
#include "../include/iniread.h"

#define VERSION "1.0"

typedef struct iniread_s {
  DARRAY sections;
  uint32_t flags;
} *INIREAD;

typedef struct token_s {
  char *token;
  char *value;
} TOKEN;

typedef struct {
  char *token;
  char *value;
  DARRAY children;
} SECTION;

static uint32_t iniread_errno = INIREAD_ERROR_SUCCESS;
static char errmsg[64] = {0};

static uint32_t parse_sections(char *buffer, INIREAD ini);
static char *goto_next_line(char *buffer);
static char *split_token(char *buffer);
static char *find_value(INIREAD ini, char *section, char *token);

INIREAD iniread_create(char *file, uint32_t flags) {
  if (!file) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    iniread_errno = INIREAD_ERROR_NULL_INPUT;
    return NULL;
  }

  INIREAD ini = malloc(sizeof(struct iniread_s));
  if (!ini) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    iniread_errno = INIREAD_ERROR_NO_MEM;
    return NULL;
  }

  FILE *fd = fopen(file, "rb");
  if (!fd) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: Error opening file.\n");

    free(ini);
    iniread_errno = INIREAD_ERROR_FILE_ERROR;
    return NULL;
  }

  fseek(fd, 0, SEEK_END);
  long file_length = ftell(fd);
  rewind(fd);

  ini->sections = darray_create(sizeof(SECTION), 0);
  if (!ini->sections) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    fclose(fd);
    free(ini);

    iniread_errno = INIREAD_ERROR_NO_MEM;
    return NULL;
  }

  char *buffer = calloc(file_length + 1, sizeof(char));
  if (!buffer) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    fclose(fd);
    free(ini);

    iniread_errno = INIREAD_ERROR_NO_MEM;
    return NULL;
  }

  if (fread(buffer, sizeof(char), file_length, fd) != file_length) {
    if (flags & INIREAD_VERBOSE)
      printf("VERBOSE: Malloc returned null.\n");

    fclose(fd);
    free(buffer);
    free(ini);

    iniread_errno = INIREAD_ERROR_FILE_ERROR;
    return NULL;
  }
  fclose(fd);

  ini->flags = flags;
  parse_sections(buffer, ini);

  if (flags & INIREAD_VERBOSE)
    printf("VERBOSE: Ini Read created.\n");

  iniread_errno = INIREAD_ERROR_SUCCESS;
  return ini;
}

uint32_t iniread_destroy(INIREAD *ini) {
  if (!ini || !*ini)
    return 0;

  uint32_t flags = (*ini)->flags;

  SECTION section;
  for (int i = 0; i < darray_get_length((*ini)->sections); i++) {
    if (darray_get_element((*ini)->sections, i, &section) == 0)
      continue;

    darray_destroy(&section.children, NULL);
  }

  darray_destroy(&(*ini)->sections, NULL);
  free(*ini);
  *ini = NULL;

  if (flags & INIREAD_VERBOSE)
    printf("VERBOSE: Ini Read destroyed.\n");

  iniread_errno = INIREAD_ERROR_SUCCESS;
  return 1;
}

uint32_t iniread_read_int(INIREAD ini, char *section, char *token,
                          int *output) {
  if (!ini || !token || !output) {
    if (ini->flags & INIREAD_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    return 0;
  }

  char *value = find_value(ini, section, token);
  if (!value)
    return 0;

  *output = strtol(value, NULL, 10);
  return 1;
}

uint32_t iniread_read_dbl(INIREAD ini, char *section, char *token,
                          double *output) {
  if (!ini || !token || !output) {
    if (ini->flags & INIREAD_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    return 0;
  }

  char *value = find_value(ini, section, token);
  if (!value)
    return 0;

  *output = strtod(value, NULL);
  return 1;
}

uint32_t iniread_read_char(INIREAD ini, char *section, char *token,
                           char **output) {
  if (!ini || !token || *output) {
    if (ini->flags & INIREAD_VERBOSE)
      printf("VERBOSE: NULL input.\n");

    return 0;
  }

  return 1;
}

uint32_t iniread_get_error() { return iniread_errno; }

char *iniread_get_error_message() {
  switch (iniread_errno) {
  case INIREAD_ERROR_SUCCESS:
    sprintf(errmsg, "Error success");
    break;
  case INIREAD_ERROR_NO_MEM:
    sprintf(errmsg, "No memory");
    break;
  case INIREAD_ERROR_NULL_INPUT:
    sprintf(errmsg, "Null input parameter");
    break;
  case INIREAD_ERROR_FILE_ERROR:
    sprintf(errmsg, "Error opening/reading provided file");
    break;
  case INIREAD_ERROR_ARRAY:
    sprintf(errmsg, "Error in external Dynamic Array library");
    break;
  }

  return errmsg;
}

char *iniread_get_version() { return VERSION; }

static uint32_t parse_sections(char *buffer, INIREAD ini) {
  if (!ini)
    return 0;

  char *line = buffer;

  while (*line != 0x0) {
    if (iscntrl(*line) || *line == '#') {
      line = goto_next_line(line);
      continue;
    }

    SECTION section = {0};
    section.token = line;
    section.value = NULL;
    section.children = NULL;

    if (*line == '[') {
      section.children = darray_create(sizeof(TOKEN), 0);
      if (!section.children) {
        // #warning Memory may be leaked if a child section fails to create an
        // array
#warning Error in parsing sections may lead to memory leaks. Will address in later version.
        // TODO: free all allocated memory
        if (ini->flags & INIREAD_VERBOSE)
          printf("VERBOSE: Error creating child array.\n");

        iniread_errno = INIREAD_ERROR_ARRAY;
        return 0;
      }
      darray_append(ini->sections, &section);

      line = goto_next_line(line);
      while (1) {
        if (*line == 0x0 || *line == '[')
          break;

        if (*line == '#' || iscntrl(*line)) {
          line = goto_next_line(line);
          continue;
        }

        TOKEN token = {0};
        token.token = line;
        token.value = split_token(line);
        while (isspace(*token.value))
          token.value++;

        darray_append(section.children, &token);
        line = goto_next_line(line);
      }

      continue;
    } else {
      section.value = split_token(line);
      while (isspace(*section.value))
        section.value++;
      darray_append(ini->sections, &section);
    }

    line = goto_next_line(line);
  }

  return 1;
}

static char *goto_next_line(char *buffer) {
  char *line = buffer;
  while (*line != '\n')
    line++;

  return line + 1;
}

static char *split_token(char *buffer) {
  char *position = buffer;

  while (*position != '=')
    position++;

  return position + 1;
}

static char *find_value(INIREAD ini, char *section, char *token) {
  SECTION line;
  int length = darray_get_length(ini->sections);

  if (section) {
    for (int i = 0; i < length; i++) {
      darray_get_element(ini->sections, i, &line);

      if (strncmp(section, line.token + 1, strlen(section)) == 0)
        goto FOUND_SECTION;
    }

    if (ini->flags)
      printf("VERBOSE: Could not find section.\n");

    return NULL;

  FOUND_SECTION:
    TOKEN child;
    length = darray_get_length(line.children);
    for (int i = 0; i < length; i++) {
      darray_get_element(line.children, i, &child);

      if (strncmp(token, child.token, strlen(token)) == 0)
        return child.value;
    }
  } else {
    for (int i = 0; i < length; i++) {
      darray_get_element(ini->sections, i, &line);

      if (line.token) {
        if (strncmp(token, line.token, strlen(token)) == 0)
          return line.value;
      }

      if (!line.children)
        continue;

      TOKEN child;
      int section_length = darray_get_length(line.children);
      for (int j = 0; j < section_length; j++) {
        darray_get_element(line.children, j, &child);

        if (strncmp(token, child.token, strlen(token)) == 0)
          return child.value;
      }
    }
  }

  if (ini->flags)
    printf("VERBOSE: Could not find token.\n");
  return NULL;
}
