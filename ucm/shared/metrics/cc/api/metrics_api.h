namespace UC::Metrics {

void SetUp(size_t maxVectorLen);

void CreateStats(const std::string& name, const std::string& type);

void UpdateStats(const std::string& name, double value);

void UpdateStats(const std::unordered_map<std::string, double>& values);

std::tuple<std::unordered_map<std::string, double>, std::unordered_map<std::string, double>,
           std::unordered_map<std::string, std::vector<double>>>
GetAllStatsAndClear();

}  // namespace UC::Metrics
#endif