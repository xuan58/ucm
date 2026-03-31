namespace UC::PosixStore {

struct Config {
    std::vector<std::string> storageBackends{};
    int32_t deviceId{-1};
    size_t tensorSize{0};
    size_t shardSize{0};
    size_t blockSize{0};
    std::string ioEngine{"psync"};  // "aio", "psync"
    bool ioDirect{false};
    std::vector<ssize_t> cpuAffinityCores{};
    size_t dataTransConcurrency{128};
    size_t lookupConcurrency{16};
    size_t openConcurrency{32};
    size_t commitConcurrency{4};
    size_t timeoutMs{30000};
    size_t dataDirShardBytes{3};
    bool posixGcEnable{true};
    double posixGcRecyclePercent{0.1};
    size_t posixGcConcurrency{16};
    size_t posixGcCheckIntervalSec{30};
    size_t posixCapacityGb{0};
    double posixGcTriggerThresholdRatio{0.7};
    size_t posixGcMaxRecycleCountPerShard{1000};
    double posixGcShardSampleRatio{0.1};
};

}  // namespace UC::PosixStore

#endif
