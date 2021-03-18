#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <imp.h>

char *imp_error_str[] = {[imp_success] = "Success",
                         [imp_eof] = "EOF",
                         [imp_unexpected_bracket] = "Unexpected Bracket"};

struct imp_parser imp_init(imp_command_callback callback, void *payload,
                           FILE *stream) {
  struct imp_parser p = {callback, payload, stream};

  p._levels = malloc(sizeof(size_t));
  p._levels_cap = 1;

  p._levels[0] = 0;
  p._levels_len = 1;

  return p;
}

void imp_destroy(struct imp_parser *p) {
  free(p->_levels);
  free(p->_argv);
  free(p->_line);
  fclose(p->stream);
}

static void push_level(struct imp_parser *p, size_t argc) {
  if (p->_levels_len + 1 > p->_levels_cap) {
    p->_levels_cap *= 2;
    p->_levels = realloc(p->_levels, sizeof(size_t) * p->_levels_cap);
  }
  p->_levels_len++;
  p->_levels[p->_levels_len - 1] = argc;
}

static void pop_level(struct imp_parser *p) { p->_levels_len--; }

static void add_arg(struct imp_parser *p, size_t *argc, char *arg) {
  // add token to argv
  if (*argc + 1 > p->_argv_cap) {
    if (p->_argv_cap == 0) {
      p->_argv_cap = 1;
    }
    p->_argv_cap *= 2;
    p->_argv = realloc(p->_argv, sizeof(char *) * p->_argv_cap);
  }

  p->_argv[*argc] = strdup(arg);
  (*argc)++;
}

// TODO: split into multiple functions
enum imp_error imp_next_command(struct imp_parser *p) {
  ssize_t len;
  len = getline(&p->_line, &p->_line_cap, p->stream);
  if (len == -1)
    return imp_eof;

  p->line_num++;

  // remove newline
  if (p->_line[len - 1] == '\n') {
    p->_line[len - 1] = '\0';
  }

  char *save_ptr;
  char *token;

  token = strtok_r(p->_line, " ", &save_ptr);

  // skip empty lines and comments
  if (token == NULL || token[0] == '#') {
    return imp_next_command(p);
  }

  if (token[0] == '{') {
    return imp_unexpected_bracket;
  }

  size_t argc = p->_levels[p->_levels_len - 1];
  while (token != NULL) {
    // if there is a comment, skip the rest of the line
    if (token[0] == '#')
      break;

    // TODO: check for arguments or characters
    if (token[0] == '{') {
      push_level(p, argc);
      return imp_next_command(p);
    }

    if (token[0] == '}') {
      pop_level(p);
      return imp_next_command(p);
    }

    add_arg(p, &argc, token);

    token = strtok_r(NULL, " ", &save_ptr);
  }

  p->callback(p->payload, p->line_num, argc, p->_argv);

  for (int i = p->_levels[p->_levels_len - 1]; i < argc; i++) {
    free(p->_argv[i]);
  }

  return imp_success;
}
