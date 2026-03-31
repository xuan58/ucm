namespace UC {

class TransManager : public TaskManager {
public:
    Status Setup(const int32_t deviceId, const size_t streamNumber, const size_t ioSize,
                 const size_t bufferNumber, const SpaceLayout* layout, const size_t timeoutMs,
                 bool useDirect = false)
    {
        this->timeoutMs_ = timeoutMs;
        auto status = Status::OK();
        for (size_t i = 0; i < streamNumber; i++) {
            auto q = std::make_shared<PosixQueue>();
            status = q->Setup(deviceId, ioSize, bufferNumber, &this->failureSet_, layout, timeoutMs,
                              useDirect);
            if (status.Failure()) { break; }
            this->queues_.emplace_back(std::move(q));
        }
        return status;
    }
};

}  // namespace UC

#endif
