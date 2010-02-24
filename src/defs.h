#ifndef DEFS_H
#define DEFS_H

#define _STRING_(x) #x
#define STRING(x) _STRING_(x)
#define STR_LINE STRING(__LINE__)

#define ERROR(message, ...) fprintf(stderr, "[ERROR] "__FILE__":"STR_LINE" - "message"\n", ## __VA_ARGS__)
#define WARN(message, ...) fprintf(stderr, "[WARN] "__FILE__":"STR_LINE" - "message"\n", ## __VA_ARGS__)
#define INFO(message, ...) fprintf(stdout, "[INFO] "__FILE__":"STR_LINE" - "message"\n", ## __VA_ARGS__)

#define EXIT exit(errno)

#define CHECK_UNLESS(condition) if (condition) {\
  ERROR(#condition);\
  EXIT;\
}

#define LOOP for (;;)

#endif

