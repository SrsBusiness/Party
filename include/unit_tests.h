#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#define malloc(size) test_malloc(size)
#define calloc(nmemb, size) test_calloc(nmemb, size)
#define realloc(ptr, size) test_realloc(ptr, size)
#define free(ptr) test_free(ptr)

