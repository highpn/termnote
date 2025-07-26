
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
extern size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
struct MemoryStruct {
    char *memory;
    size_t size;
};