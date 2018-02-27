
#include "log.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <unistd.h>

#if defined(DEBUG) || defined(LOGGING)

ostream log_file()
{
    char buf[BUFSIZ + 1] = {0};

    snprintf(buf, BUFSIZ, "yahtsee.%d.log", getpid());

    fostream out(buf, "a");

    if (!out.is_open()) {
        perror("opening log file");
        return;
    }

    return out;
}

#endif
