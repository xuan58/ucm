namespace UC::PosixStore {

class TransQueue {
    using TaskIdSet = HashSet<Detail::TaskHandle>;
    using TaskPtr = std::shared_ptr<TransTask>;
    using WaiterPtr = std::shared_ptr<Latch>;

private:
    struct IoUnit {
        Detail::TaskHandle owner;
        Detail::Shard shard;
        std::shared_ptr<Latch> waiter;
        bool firstIo{false};
    };
    TaskIdSet* failureSet_;
    const SpaceLayout* layout_;
    ThreadPool<IoUnit> loadPool_;
    ThreadPool<IoUnit> dumpPool_;
    size_t ioSize_;
    size_t shardSize_;
    size_t nShardPerBlock_;
    bool ioDirect_;
    size_t timeoutMs_;

public:
    Status Setup(const Config& config, TaskIdSet* failureSet, const SpaceLayout* layout);
    void Push(TaskPtr task, WaiterPtr waiter);
    void Cancel(TaskPtr task);

private:
    void LoadWorker(IoUnit& ios);
    void DumpWorker(IoUnit& ios);
    void OnIoUnitTimeout(IoUnit& ios);
    Status H2S(IoUnit& ios);
    Status S2H(IoUnit& ios);
};

}  // namespace UC::PosixStore

#endif
