namespace UC::CacheStore {

class DumpQueue {
    using TaskPtr = std::shared_ptr<TransTask>;
    using WaiterPtr = std::shared_ptr<Latch>;
    using TaskPair = std::pair<TaskPtr, WaiterPtr>;
    using TaskIdSet = HashSet<Detail::TaskHandle>;
    struct DumpCtx {
        Detail::TaskHandle taskHandle;
        Detail::TaskHandle backendTaskHandle;
        std::vector<TransBuffer::Handle> bufferHandles;
    };

private:
    alignas(64) std::atomic_bool stop_{false};
    Detail::TaskHandle finishedBackendTaskHandle_{0};
    TaskIdSet* failureSet_{nullptr};
    TransBuffer* buffer_{nullptr};
    StoreV1* backend_{nullptr};
    int32_t deviceId_{-1};
    std::vector<size_t> tensorSizes_{};
    size_t streamNumber_{1};
    std::vector<ssize_t> cpuAffinityCores_{};
    SpscRingQueue<TaskPair> waiting_;
    SpscRingQueue<DumpCtx> dumping_;
    std::thread dispatcher_;
    std::thread dumper_;

public:
    ~DumpQueue();
    Status Setup(const Config& config, TaskIdSet* failureSet, TransBuffer* buffer);
    void Submit(TaskPtr task, WaiterPtr waiter);

private:
    void DispatchStage(std::promise<Status>& started);
    void DispatchOneTask(CopyStream& stream, TaskPair&& pair);
    Status DumpOneTask(CopyStream& stream, TaskPtr task);
    Status DeviceToHostGatherAsync(std::shared_ptr<Trans::Stream> stream, void** device,
                                   void* host);
    void BackendDumpStage();
};

}  // namespace UC::CacheStore

#endif
