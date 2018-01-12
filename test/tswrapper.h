#include "csapp.h"

extern sem_t *ts_hnmutex;

struct hostent *gethostbyname_ts(const char *info, struct hostent *res);

void tswrapper_init();

void tswrapper_deinit();