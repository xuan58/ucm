namespace UC::Logger {

void Log(Level lv, std::string file, std::string func, int line, std::string msg)
{
    Logger::GetInstance().Log(std::move(lv), SourceLocation{file.c_str(), func.c_str(), line},
                              std::move(msg));
}

void LogRateLimit(Level lv, std::string file, std::string func, int line, std::string msg)
{
    if (Logger::GetInstance().FilterCallSite(file.c_str(), line)) {
        Logger::GetInstance().Log(std::move(lv), SourceLocation{file.c_str(), func.c_str(), line},
                                  std::move(msg));
    }
}

void Setup(const std::string& path, int max_files, int max_size)
{
    Logger::GetInstance().Setup(path, max_files, max_size);
}

void Flush() { Logger::GetInstance().Flush(); }

bool isEnabledFor(Level lv) { return Logger::GetInstance().IsEnabledFor(lv); }

}  // namespace UC::Logger