#ifndef __CACHE_H__
#define __CACHE_H__

#include <time.h>
#include "crc32.h"
#include "csapp.h"

#define MAX_CACHE_SIZE  (1 << 20) // 1M cache size
#define MAX_OBJECT_SIZE 102400 // 100k max object size

struct cache_entry_t {
    int key;
    char *content;
    size_t content_size;
    time_t timestamp;
    struct cache_entry_t *next_entry;
    struct cache_entry_t *prev_entry;
};

struct {
    struct cache_entry_t *ca_head;
    struct cache_entry_t *ca_tail;
    size_t ca_size;
} cache;

typedef struct cache_entry_t CE;

void cache_init();
void cache_deinit();
CE *is_in_cache(char *request);
void remove_cache_entry(CE *entry);
CE *add_cache_entry(char *request, char *content, int len);
void check_cache();
void update_cache(CE *entry);

#endif