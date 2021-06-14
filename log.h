#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#if !defined LOGGING_MACROS
extern char log_buff[255];
#define LOG_DEBUG(...) sprintf(log_buff, __VA_ARGS__); root.debug(log_buff);
#define LOG_INFO(...) sprintf(log_buff, __VA_ARGS__); root.info(log_buff);
#define LOG_WARN(...) sprintf(log_buff, __VA_ARGS__); root.warn(log_buff);
#define LOG_ERROR(...) sprintf (log_buff, __VA_ARGS__); root.error(log_buff);
#define LOGGING_MACROS
#endif

extern log4cpp::Category& root;