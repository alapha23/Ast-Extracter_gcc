#include "cache.h"
#include "assert.h"
#include <stdio.h>

sem_t *mutexp;
sem_t *w;
int ca_readcnt = 0;

void cache_init() 
{
    ca_readcnt = 0;

    sem_unlink("mutexp");
    sem_unlink("w");

    mutexp = sem_open("mutexp", O_CREAT, DEF_MODE, 1);
    w = sem_open("w", O_CREAT, DEF_MODE, 1);

    cache.ca_head = cache.ca_tail = NULL;
    cache.ca_size = 0;
    printf("init cache\n");
}

void cache_deinit()
{
    CE *ptr = cache.ca_head;
    sem_destroy(mutexp);
    sem_destroy(w);

    if (ptr == NULL) return;

    CE *next = ptr->next_entry;
    while (ptr) {
        free(ptr->content);
        free(ptr);
        ptr = next;
        if (ptr)
            next = ptr->next_entry;
    }
}

CE *is_in_cache(char *request) {
    sem_wait(mutexp);
    check_cache();
    printf("Finding cache\n");
    ca_readcnt++;
    if (ca_readcnt == 1)
        sem_wait(w);
    sem_post(mutexp);
    CE *res = cache.ca_tail;
    int key = crc32(request);

    while (res) {
        if (key == res->key) {
            printf("Cache hit! %s\n", request);
            return res;
        }
        res = res->prev_entry;
    }

    sem_wait(mutexp);
    ca_readcnt--;
    if (ca_readcnt == 0)
        sem_post(w);
    sem_post(mutexp);
    if (res == NULL)
        printf("cache not found\n");
    return res;
}
void remove_cache_entry(CE *entry) {
    printf("Remove cache entry\n");
    if (entry == NULL) return;
    int size = entry->content_size;
    printf("free cache entry of size %d\n", size);
    Free(entry->content);

    if (entry == cache.ca_head) {
        cache.ca_head = entry->next_entry;
        if (cache.ca_head)
            cache.ca_head->prev_entry = NULL;
        Free(entry);
    } else if (entry == cache.ca_tail) {
        cache.ca_tail = entry->prev_entry;
        if (cache.ca_tail)
            cache.ca_tail->next_entry = NULL;
        Free(entry);
    } else {
        entry->prev_entry->next_entry = entry->next_entry;
        entry->next_entry->prev_entry = entry->prev_entry;
        Free(entry);
    }
    cache.ca_size -= size;
    check_cache();
}


CE *add_cache_entry(char *request, char *content, int len) {
    sem_wait(w);

    check_cache();
    fflush(stdout);
    CE *res;
    // Rio_writen(1, content, len);
    printf("add object of size %d -- ", len);
    while (cache.ca_size + len > MAX_CACHE_SIZE) {
        printf("1\n");
        remove_cache_entry(cache.ca_head);
    }
    cache.ca_size += len;
    printf("cache size: %ld\n", cache.ca_size);
    printf("%s\n", request);

    if (cache.ca_head == NULL) {
        cache.ca_head = Malloc(sizeof(CE));
        cache.ca_tail = cache.ca_head;
        res = cache.ca_head;

        res->content = Malloc(len);
        memcpy(res->content, content, len);
        res->content_size = len;
        res->next_entry = NULL;
        res->prev_entry = NULL;
        res->key = crc32(request);
    } else {
        cache.ca_tail->next_entry = Malloc(sizeof(CE));
        res = cache.ca_tail->next_entry;

        res->content = Malloc(len);
        memcpy(res->content, content, len);
        res->content_size = len;
        res->next_entry = NULL;
        res->prev_entry = cache.ca_tail;
        res->key = crc32(request);
        cache.ca_tail = res;
    }
    check_cache();
    sem_post(w);
    printf("added object of size %d\n", len);
    return res;
}

void update_cache(CE *entry) {
    sem_wait(w);
    printf("cache updatint\n");
    check_cache();
    if (entry->next_entry != NULL) {
        if (entry->prev_entry != NULL) {
            entry->next_entry->prev_entry = entry->prev_entry;
            entry->prev_entry->next_entry = entry->next_entry;
            entry->next_entry = NULL;
            entry->prev_entry = cache.ca_tail;
            cache.ca_tail->next_entry = entry;
            cache.ca_tail = entry;
        } else {
            entry->next_entry->prev_entry = NULL;
            cache.ca_head = entry->next_entry;
            entry->next_entry = NULL;
            entry->prev_entry = cache.ca_tail;
            cache.ca_tail->next_entry = entry;
            cache.ca_tail = entry;
        }
    }
    check_cache();
    sem_post(w);
    printf("cache updated\n");
}

void check_cache() {
    return ;
    CE *ptr = cache.ca_head;
    int size = 0;
    while (ptr) {
        if (ptr->next_entry != NULL) {
            assert(ptr == ptr->next_entry->prev_entry);
        }
        size += ptr->content_size;
        ptr = ptr->next_entry;
    }
    assert(size == cache.ca_size);
    if (cache.ca_head != NULL) {
        assert(cache.ca_tail != NULL);
        assert(cache.ca_head->prev_entry == NULL);
        assert(cache.ca_tail->next_entry == NULL);
    }
    printf("check passed\n");
}