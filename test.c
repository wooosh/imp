// #pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <imp.h>

void callback(void* filename, size_t line, size_t argc, char **argv) {
  printf("%s:%zu: ", filename, line);
  for (int i = 0; i < argc; i++) {
    printf(" %s", argv[i]);
  }
  printf("\n");
}

void main(int argc, char **argv) {
  if (argc != 2) {
    printf("no\n");
    exit(1);
  }

  FILE *f = fopen(argv[1], "r");
  struct imp_parser p = imp_init(callback, argv[1], f);

  enum imp_error err;
  while ((err = imp_next_command(&p)) == imp_success) {};

  if (err != imp_eof) {
    printf("%s:%d: %s\n", argv[1], p.line_num, imp_error_str[err]);
  }

  imp_destroy(&p); 
}
