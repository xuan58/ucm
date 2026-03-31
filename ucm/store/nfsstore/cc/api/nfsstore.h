namespace UC {

class NFSStore : public CCStore<> {
public:
    struct Config {
        std::vector<std::string> storageBackends;
        size_t kvcacheBlockSize;
        bool transferEnable;
        int32_t transferDeviceId;
        size_t transferStreamNumber;
        size_t transferIoSize;
        size_t transferBufferNumber;
        size_t transferTimeoutMs;
        bool tempDumpDirEnable;
        bool hotnessEnable;
        size_t hotnessInterval;
        size_t storageCapacity;
        bool recycleEnable;
        float recycleThresholdRatio;
        bool transferIoDirect;

        Config(const std::vector<std::string>& storageBackends, const size_t kvcacheBlockSize,
               const bool transferEnable)
            : storageBackends{storageBackends}, kvcacheBlockSize{kvcacheBlockSize},
              transferEnable{transferEnable}, transferDeviceId{-1}, transferStreamNumber{32},
              transferIoSize{262144}, transferBufferNumber{512}, transferTimeoutMs{30000},
              tempDumpDirEnable{false}, hotnessEnable{true}, hotnessInterval{60},
              storageCapacity{0}, recycleEnable{true}, recycleThresholdRatio{0.7f},
              transferIoDirect{false}
        {
        }
    };

public:
    NFSStore() : impl_{nullptr} {}
    ~NFSStore() override
    {
        if (this->impl_) { delete this->impl_; }
    }
    int32_t Setup(const Config& config);
    int32_t Alloc(const std::string& block) override { return this->impl_->Alloc(block); }
    bool Lookup(const std::string& block) override { return this->impl_->Lookup(block); }
    void Commit(const std::string& block, const bool success) override
    {
        this->impl_->Commit(block, success);
    }
    std::list<int32_t> Alloc(const std::list<std::string>& blocks) override
    {
        return this->impl_->Alloc(blocks);
    }
    std::list<bool> Lookup(const std::list<std::string>& blocks) override
    {
        return this->impl_->Lookup(blocks);
    }
    void Commit(const std::list<std::string>& blocks, const bool success) override
    {
        this->impl_->Commit(blocks, success);
    }
    size_t Submit(Task&& task) override { return this->impl_->Submit(std::move(task)); }
    int32_t Wait(const size_t task) override { return this->impl_->Wait(task); }
    int32_t Check(const size_t task, bool& finish) override
    {
        return this->impl_->Check(task, finish);
    }

private:
    NFSStore* impl_;
};

}; // namespace UC

#endif
