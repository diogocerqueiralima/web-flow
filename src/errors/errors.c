#include "errors.h"

char *get_severity_message(Severity level) {

  switch (level) {
    case INFO:
      return "INFO";
    case CRITICAL:
      return "CRITICAL";
    case FATAL:
      return "FATAL";
    default:
      fprintf(stderr, "[FATAL] Invalid severy level\n");
      exit(EXIT_FAILURE);
  }

}

void throw_system_error(Severity level, char *msg) {

  char *severityMessage = get_severity_message(level);

  fprintf(stderr, "[%s] ", severityMessage);

  if (errno) {
    perror(msg);
  }else {
    fprintf(stderr, "%s\n", msg);
  }

  errno = 0;

  if (level == FATAL)
    exit(EXIT_FAILURE);

}
