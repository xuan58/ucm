namespace UC::Ds3fsStore {

struct Config {
    std::vector<std::string> storageBackends{};
    int32_t deviceId{-1};
    size_t tensorSize{0};
    size_t shardSize{0};
    size_t blockSize{0};
    bool ioDirect{true};
    size_t streamNumber{32};
    size_t timeoutMs{30000};
    size_t iorEntries{1};
    int32_t iorDepth{1};
    int32_t numaId{-1};
};

}  // namespace UC::Ds3fsStore

#endif
