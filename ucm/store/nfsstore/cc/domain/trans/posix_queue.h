namespace UC {

class PosixQueue : public TaskQueue {
    using Device = std::unique_ptr<IDevice>;
    int32_t deviceId_{-1};
    size_t bufferSize_{0};
    size_t bufferNumber_{0};
    TaskSet* failureSet_{nullptr};
    const SpaceLayout* layout_{nullptr};
    bool useDirect_{false};
    ThreadPool<Task::Shard, Device> backend_{};

public:
    Status Setup(const int32_t deviceId, const size_t bufferSize, const size_t bufferNumber,
                 TaskSet* failureSet, const SpaceLayout* layout, const size_t timeoutMs,
                 bool useDirect = false);
    void Push(std::list<Task::Shard>& shards) noexcept override;

private:
    bool Init(Device& device);
    void Exit(Device& device);
    void Work(Task::Shard& shard, const Device& device);
    void Done(Task::Shard& shard, const Device& device, const bool success);
    Status D2S(Task::Shard& shard, const Device& device);
    Status S2D(Task::Shard& shard, const Device& device);
    Status H2S(Task::Shard& shard);
    Status S2H(Task::Shard& shard);
};

}  // namespace UC

#endif
