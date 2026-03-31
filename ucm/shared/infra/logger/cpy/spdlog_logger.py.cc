namespace py = pybind11;
namespace UC::Logger {

void LogWrapper(Level lv, std::string file, std::string func, int line, std::string msg)
{
    Log(std::move(lv), std::move(file), std::move(func), line, std::move(msg));
}

void RateLimitLogWrapper(Level lv, std::string file, std::string func, int line, std::string msg)
{
    LogRateLimit(std::move(lv), std::move(file), std::move(func), line, std::move(msg));
}

PYBIND11_MODULE(ucmlogger, m)
{
    m.def("setup", &Setup);
    m.def("flush", &Flush);
    m.def("log", &LogWrapper);
    m.def("log_rate_limit", &RateLimitLogWrapper);
    m.def("isEnabledFor", &isEnabledFor);
    py::enum_<Level>(m, "Level")
        .value("DEBUG", Level::DEBUG)
        .value("INFO", Level::INFO)
        .value("WARNING", Level::WARN)
        .value("ERROR", Level::ERROR)
        .value("CRITICAL", Level::CRITICAL)
        .value("FATAL", Level::CRITICAL);
}

}  // namespace UC::Logger