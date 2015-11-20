
#ifndef _YAHTSEE_GAME_H_
#define _YAHTSEE_GAME_H_

#if !defined(DEBUG) && !defined(LOGGING)

#define logstr(args...)

#else

#ifndef __attribute__
#define __attribute__(x)
#endif

void log_str(const char *const format, ...) __attribute__((format(printf, 1, 2)));

#define logstr(args...) log_str(args)

#endif

#endif
