namespace UC::PosixStore {

class SpaceManager {
    struct PrefixLookupContext {
        const Detail::BlockId* blocks;
        size_t begin;
        size_t end;
        size_t nWorker;
        std::shared_ptr<std::atomic<ssize_t>> firstFail;
        std::shared_ptr<std::atomic<int32_t>> status;
        std::shared_ptr<Latch> waiter;
    };

private:
    SpaceLayout layout_;
    ThreadPool<PrefixLookupContext> prefixLookupSrv_;
    HotnessTracker hotnessTracker_;
    ShardGarbageCollector gcMgr_;
    bool gcEnable_{false};

public:
    Status Setup(const Config& config);
    Expected<std::vector<uint8_t>> Lookup(const Detail::BlockId* blocks, size_t num);
    Expected<ssize_t> LookupOnPrefix(const Detail::BlockId* blocks, size_t num);
    const SpaceLayout* GetLayout() const { return &layout_; }

private:
    uint8_t Lookup(const Detail::BlockId* block);
    void OnLookupPrefix(PrefixLookupContext& ctx);
    void OnLookupPrefixTimeout(PrefixLookupContext& ctx);
};

}  // namespace UC::PosixStore

#endif
