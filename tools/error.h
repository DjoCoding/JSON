#ifndef ERROR_H
#define ERROR_H

#define REPORT_ERROR(...) { fprintf(stderr, "[ERROR]: " __VA_ARGS__); fprintf(stderr, "\n"); }
#define EXIT_ERROR(...) {  REPORT_ERROR(__VA_ARGS__); exit(EXIT_FAILURE); }
#define DEBUG_ERROR(...) { fprintf(stderr, __FILE__ "::%d -> ", __LINE__); fprintf(stderr, "[ERROR]: " __VA_ARGS__); fprintf(stderr, "\n"); exit(EXIT_FAILURE); }

#define LOG_INFO(...) { printf("[INFO]" __VA_ARGS__); }

#define ASSERT(condition, ...) \
    do { \
        if(condition) { break; } \
        fprintf(stderr, __FILE__ "::%d -> ", __LINE__); \
        fprintf(stderr, "[ASSERT]: (" #condition ") " __VA_ARGS__);  \
        fprintf(stderr, "\n"); \
        exit(EXIT_FAILURE); \
    } while(0)

#endif // ERROR_H