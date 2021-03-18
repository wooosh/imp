// #pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// TODO: should take payload
// TODO: should take line number
typedef void (*imp_command_callback)(void* payload, size_t line, size_t argc, char **argv);

enum imp_error {
  imp_success = 0,
  imp_eof,
  imp_unexpected_bracket,
  // imp_unexpected_token_after_bracket  
  // for unmatched closing brackets or closing brackets on an empty line
  // imp_expected_closing_bracket,
};

extern char *imp_error_str[];

struct imp_parser {
  imp_command_callback callback;
  void *payload;
  FILE *stream;
  size_t line_num; // read only
  
  // stores the bracket nesting level
  size_t *_levels;
  size_t _levels_len;
  size_t _levels_cap;

  char **_argv;
  size_t _argv_cap;

  char* _line;
  size_t _line_cap;
};

void imp_destroy(struct imp_parser *p);
struct imp_parser imp_init(imp_command_callback callback, void *payload, FILE* stream);
enum imp_error imp_next_command(struct imp_parser *p);

