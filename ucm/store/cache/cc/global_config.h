namespace UC::CacheStore {

struct Config {
    StoreV1* storeBackend{};
    std::string uniqueId{};
    int32_t deviceId{-1};
    std::vector<size_t> tensorSizes{};
    size_t shardSize{0};
    size_t blockSize{0};
    bool ioDirect{false};
    std::vector<ssize_t> cpuAffinityCores{};
    size_t bufferCapacity{256ULL << 30};
    bool shareBufferEnable{true};
    size_t waitingQueueDepth{8192};
    size_t runningQueueDepth{524288};
    size_t timeoutMs{30000};
    size_t streamNumber{4};
};

}  // namespace UC::CacheStore

#endif
