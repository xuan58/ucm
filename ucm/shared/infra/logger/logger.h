namespace UC::Logger {

void Log(Level lv, std::string file, std::string func, int line, std::string msg);
void LogRateLimit(Level lv, std::string file, std::string func, int line, std::string msg);

template <typename... Args>
void Log(Level lv, const SourceLocation& loc, fmt::format_string<Args...> fmt, Args&&... args)
{
    std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
    Log(lv, std::string(loc.file), std::string(loc.func), loc.line, std::move(msg));
}

template <typename... Args>
void LogRateLimit(Level lv, const SourceLocation& loc, fmt::format_string<Args...> fmt,
                  Args&&... args)
{
    std::string msg = fmt::format(fmt, std::forward<Args>(args)...);
    LogRateLimit(lv, std::string(loc.file), std::string(loc.func), loc.line, std::move(msg));
}

void Setup(const std::string& path, int max_files, int max_size);
void Flush();
bool isEnabledFor(Level lv);

}  // namespace UC::Logger
#define UC_SOURCE_LOCATION {__FILE__, __FUNCTION__, __LINE__}
#define UC_LOG_UNLIMITED(lv, fmt, ...) \
    UC::Logger::Log(lv, UC_SOURCE_LOCATION, FMT_STRING(fmt), ##__VA_ARGS__)
#define UC_LOG(lv, fmt, ...) \
    UC::Logger::LogRateLimit(lv, UC_SOURCE_LOCATION, FMT_STRING(fmt), ##__VA_ARGS__)
#define UC_DEBUG_UNLIMITED(fmt, ...) UC_LOG_UNLIMITED(UC::Logger::Level::DEBUG, fmt, ##__VA_ARGS__)
#define UC_INFO_UNLIMITED(fmt, ...) UC_LOG_UNLIMITED(UC::Logger::Level::INFO, fmt, ##__VA_ARGS__)
#define UC_WARN_UNLIMITED(fmt, ...) UC_LOG_UNLIMITED(UC::Logger::Level::WARN, fmt, ##__VA_ARGS__)
#define UC_ERROR_UNLIMITED(fmt, ...) UC_LOG_UNLIMITED(UC::Logger::Level::ERROR, fmt, ##__VA_ARGS__)
#define UC_DEBUG(fmt, ...) UC_LOG(UC::Logger::Level::DEBUG, fmt, ##__VA_ARGS__)
#define UC_INFO(fmt, ...) UC_LOG(UC::Logger::Level::INFO, fmt, ##__VA_ARGS__)
#define UC_WARN(fmt, ...) UC_LOG(UC::Logger::Level::WARN, fmt, ##__VA_ARGS__)
#define UC_ERROR(fmt, ...) UC_LOG(UC::Logger::Level::ERROR, fmt, ##__VA_ARGS__)
#endif
