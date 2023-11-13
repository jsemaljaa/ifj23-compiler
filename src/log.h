#ifndef LOG_H
#define LOG_H

#define DEBUG 1
#define INFO 2
#define WARNING 3
#define ERROR 4

#ifndef LOG_LEVEL
    #define LOG_LEVEL INFO
#endif

#define _log(prefix, fmt, ...) \
    fprintf(stderr, "[" prefix ", %s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);

#define debug(fmt, ...) \
    if(LOG_LEVEL == DEBUG) _log("DEBUG", fmt, ##__VA_ARGS__);

#define info(fmt, ...) \
    if(LOG_LEVEL <= INFO) _log("INFO", fmt, ##__VA_ARGS__);

#define warning(fmt, ...) \
if(LOG_LEVEL <= WARNING) _log("WARNING", fmt, ##__VA_ARGS__);

#define error(fmt, ...) \
if(LOG_LEVEL <= ERROR) _log("ERROR", fmt, ##__VA_ARGS__);

#endif // LOG_h