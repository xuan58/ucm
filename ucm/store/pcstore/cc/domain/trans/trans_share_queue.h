namespace UC {

class TransShareQueue {
    using TaskPtr = std::shared_ptr<TransTask>;
    using WaiterPtr = std::shared_ptr<TaskWaiter>;
    struct BlockTask {
        std::shared_ptr<ShareBuffer::Reader> reader;
        size_t owner;
        std::vector<uintptr_t> shards;
        std::function<void(bool)> done;
    };
    int32_t deviceId_;
    size_t streamNumber_;
    size_t ioSize_;
    ShareBuffer buffer_;
    const SpaceLayout* layout_;
    TaskSet* failureSet_;
    std::atomic_bool stop_{false};
    std::mutex mutex_;
    std::condition_variable cv_;
    std::list<BlockTask> load_;
    std::list<BlockTask> wait_;
    std::list<std::thread> threads_;

public:
    ~TransShareQueue();
    Status Setup(const int32_t deviceId, const size_t streamNumber, const size_t blockSize,
                 const size_t ioSize, const bool ioDirect, const size_t bufferNumber,
                 const SpaceLayout* layout, TaskSet* failureSet, const std::string& uniqueId);
    void Dispatch(TaskPtr task, WaiterPtr waiter);

private:
    void WorkerLoop(std::promise<Status>& status);
    void Worker(Trans::Stream& stream);
    void HandleReadyTask(Status s, BlockTask& task, Trans::Stream& stream);
    void HandleLoadTask(BlockTask& task, Trans::Stream& stream);
};

}  // namespace UC

#endif
