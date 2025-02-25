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

  void throw_system_error(Severity level, char *msg);

#endif
