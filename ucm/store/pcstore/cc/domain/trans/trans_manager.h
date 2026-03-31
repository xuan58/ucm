namespace UC {

class TransManager {
public:
    Status Setup(const size_t rankSize, const int32_t deviceId, const size_t streamNumber,
                 const size_t blockSize, const size_t ioSize, const bool ioDirect,
                 const size_t bufferNumber, const SpaceLayout* layout, const size_t timeoutMs,
                 const bool scatterGatherEnable, const std::string& uniqueId);
    Status Submit(TransTask task, size_t& taskId) noexcept;
    Status Wait(const size_t taskId) noexcept;
    Status Check(const size_t taskId, bool& finish) noexcept;

private:
    using TaskPtr = std::shared_ptr<TransTask>;
    using WaiterPtr = std::shared_ptr<TaskWaiter>;
    using TaskPair = std::pair<TaskPtr, WaiterPtr>;
    TransShareQueue shareQueue_;
    TransQueue queue_;
    size_t rankSize_;
    size_t timeoutMs_;
    std::mutex mutex_;
    std::unordered_map<size_t, TaskPair> tasks_;
    TaskSet failureSet_;
};

}  // namespace UC

#endif
