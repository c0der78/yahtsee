
#include "log.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <unistd.h>

#ifdef DEBUG

void log_str(const char *const format, ...)
{
    va_list args;
    char buf[BUFSIZ + 1] = {0};
    FILE *out;

    snprintf(buf, BUFSIZ, "yahtsee.%d.log", getpid());

    out = fopen(buf, "a");

    if (!out)
    {
        perror("opening log file");
        return;
    }

    time_t t = time(0);
    strftime(buf, BUFSIZ, "%F %T ", localtime(&t));
    fputs(buf, out);
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    fputs("\n", out);
    fflush(out);
    fclose(out);
}

#endif
