
#ifndef _YAHTSEE_LOG_H_
#define _YAHTSEE_LOG_H_

#if !defined(DEBUG) && !defined(LOGGING)

#define log_trace(args...)
#define log_warn(args...)

#else

#include <arg3log/log.h>

extern ostream log_file();

#define log_trace(args...) log::trace(log_file(), args)
#define log_warn(args...) log::warn(log_file(), args)

#endif

#endif
