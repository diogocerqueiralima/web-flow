#ifndef ERRORS_H

  #define ERRORS_H

  #include <stdio.h>
  #include <stdlib.h>
  #include <errno.h>

  typedef enum {

    INFO,
    CRITICAL,
    FATAL

  } Severity;

  void throw_runtime_error(char *msg);

#endif
