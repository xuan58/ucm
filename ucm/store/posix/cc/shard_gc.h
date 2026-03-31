namespace UC::PosixStore {

struct ShardTaskContext {
    enum class Type { GC, SAMPLE };
    Type type;
    std::string shard;
    std::shared_ptr<Latch> waiter;
    std::atomic<size_t>* sampledFiles{nullptr};
    std::atomic<bool>* gcLimited{nullptr};
};

class ShardGarbageCollector {
public:
    ShardGarbageCollector() = default;
    ShardGarbageCollector(const ShardGarbageCollector&) = delete;
    ShardGarbageCollector& operator=(const ShardGarbageCollector&) = delete;
    ~ShardGarbageCollector();
    Status Setup(const SpaceLayout* layout, const Config& config);

private:
    Status ValidateAndInitCapacity();
    bool Execute();
    std::tuple<bool, size_t, size_t> ShouldTrigger();
    void ProcessTask(ShardTaskContext& ctx);
    void GCCheckLoop();
    void StopBackgroundCheck();
    const SpaceLayout* layout_{nullptr};
    Config config_;
    size_t maxFileCount_{0};
    ThreadPool<ShardTaskContext> gcPool_;
    std::thread gcCheckWorker_;
    std::mutex gcCheckMtx_;
    std::condition_variable gcCheckCv_;
    std::atomic<bool> stop_{false};
};

}  // namespace UC::PosixStore

#endif
