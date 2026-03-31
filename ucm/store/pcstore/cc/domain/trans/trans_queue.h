namespace UC {

class TransQueue {
    using TaskPtr = std::shared_ptr<TransTask>;
    using WaiterPtr = std::shared_ptr<TaskWaiter>;
    struct BlockTask {
        size_t owner;
        std::string block;
        TransTask::Type type;
        std::vector<uintptr_t> shards;
        std::shared_ptr<void> buffer;
        std::function<void(bool)> done;
    };
    void DeviceWorker(BlockTask&& task);
    void FileWorker(BlockTask& task);
    void FileWorkerTimeout(BlockTask& task);

public:
    Status Setup(const int32_t deviceId, const size_t streamNumber, const size_t blockSize,
                 const size_t ioSize, const bool ioDirect, const size_t bufferNumber,
                 const SpaceLayout* layout, TaskSet* failureSet_, const bool scatterGatherEnable,
                 const size_t timeoutMs);
    void Dispatch(TaskPtr task, WaiterPtr waiter);
    void DispatchDump(TaskPtr task, WaiterPtr waiter);
    void DispatchSatterGatherDump(TaskPtr task, WaiterPtr waiter);

private:
    std::unique_ptr<Trans::Buffer> buffer_{nullptr};
    std::unique_ptr<Trans::Stream> stream_{nullptr};
    std::unique_ptr<Trans::Buffer> devBuffer_{nullptr};
    std::unique_ptr<Trans::Stream> smStream_{nullptr};
    const SpaceLayout* layout_;
    size_t ioSize_;
    bool ioDirect_;
    ThreadPool<BlockTask> devPool_;
    ThreadPool<BlockTask> filePool_;
    TaskSet* failureSet_;
    bool scatterGatherEnable_;
};

}  // namespace UC

#endif
