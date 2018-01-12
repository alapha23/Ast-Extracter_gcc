#include "csapp.h"
#include "tswrapper.h"
sem_t *ts_hnmutex;

/* 
 * gethostbyname_ts - thread safe wrapper for gethostbyname function.
 *                    use deep copy of a global pointer to avoid race.
 */

struct hostent *gethostbyname_ts(const char *info, struct hostent *res)
{
    struct hostent *sharedp;
 
    sem_wait(ts_hnmutex);
    sharedp = gethostbyname(info);
    memcpy(res, sharedp, sizeof(struct hostent));
    sem_post(ts_hnmutex);

    return res;
}

/*
 * tswrapper_init - init routine for tswrapper functions, maily init the mutex
 */

void tswrapper_init()
{
    sem_unlink("hnmutex");
    ts_hnmutex = sem_open("hnmutex", O_CREAT, DEF_MODE, 1);
}

/*
 * tswrapper_deinit - deinit routine for tswrapper functions, maily destroy the mutex
 *                    and unlink it
 */
void tswrapper_deinit()
{   
    sem_destroy(ts_hnmutex);
    sem_unlink("hnmutex");
}