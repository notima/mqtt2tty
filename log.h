#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <errno.h>
#include <string.h>

#if !defined LOGGING_MACROS
extern char log_buff[512];
extern char err_buff[512];
#define LOG_DEBUG(...) sprintf(log_buff, __VA_ARGS__); root.debug(log_buff);
#define LOG_INFO(...) sprintf(log_buff, __VA_ARGS__); root.info(log_buff);
#define LOG_WARN(...) sprintf(log_buff, __VA_ARGS__); root.warn(log_buff);
#define LOG_ERROR(...) sprintf(log_buff, __VA_ARGS__); sprintf(err_buff, "%s: %s", log_buff, errno > 0 ? strerror(errno) : ""); root.error(err_buff);
#define LOGGING_MACROS
#endif

extern log4cpp::Category& root;