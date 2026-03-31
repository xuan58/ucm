namespace UC::Metrics {

void SetUp(size_t maxVectorLen) { Metrics::GetInstance().SetUp(maxVectorLen); }

void CreateStats(const std::string& name, const std::string& type)
{
    Metrics::GetInstance().CreateStats(name, type);
}

void UpdateStats(const std::string& name, double value)
{
    Metrics::GetInstance().UpdateStats(name, value);
}

void UpdateStats(const std::unordered_map<std::string, double>& values)
{
    Metrics::GetInstance().UpdateStats(values);
}

std::tuple<std::unordered_map<std::string, double>, std::unordered_map<std::string, double>,
           std::unordered_map<std::string, std::vector<double>>>
GetAllStatsAndClear()
{
    return Metrics::GetInstance().GetAllStatsAndClear();
}

}  // namespace UC::Metrics
