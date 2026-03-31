namespace UC::FakeStore {

class FakeStore : public StoreV1 {
    MetaManager metaMgr_;

public:
    Status Setup(const Detail::Dictionary& inConfig) override
    {
        Config config;
        inConfig.Get("unique_id", config.uniqueId);
        inConfig.GetNumber("buffer_number", config.bufferNumber);
        inConfig.Get("share_buffer_enable", config.shareBufferEnable);
        auto s = CheckConfig(config);
        if (s.Failure()) [[unlikely]] {
            UC_ERROR("Failed to check config params: {}.", s);
            return s;
        }
        s = metaMgr_.Setup(config);
        if (s.Failure()) [[unlikely]] { return s; }
        ShowConfig(config);
        return Status::OK();
    }
    std::string Readme() const override { return "FakeStore"; }
    Expected<std::vector<uint8_t>> Lookup(const Detail::BlockId* blocks, size_t num) override
    {
        std::vector<uint8_t> founds(num);
        StopWatch sw;
        std::transform(blocks, blocks + num, founds.begin(),
                       [this](const Detail::BlockId& block) { return metaMgr_.Exist(block); });
        UC_DEBUG("Fake lookup({}) costs {:.3f}ms.", num, sw.Elapsed().count() * 1e3);
        return founds;
    }
    Expected<ssize_t> LookupOnPrefix(const Detail::BlockId* blocks, size_t num) override
    {
        ssize_t index = -1;
        StopWatch sw;
        for (size_t i = 0; i < num && metaMgr_.Exist(blocks[i]); i++) {
            index = static_cast<ssize_t>(i);
        }
        UC_DEBUG("Fake Lookup({}/{}) costs {:.3f}ms.", index, num, sw.Elapsed().count() * 1e3);
        return index;
    }
    void Prefetch(const Detail::BlockId* blocks, size_t num) override {}
    Expected<Detail::TaskHandle> Load(Detail::TaskDesc task) override { return NextId(); }
    Expected<Detail::TaskHandle> Dump(Detail::TaskDesc task) override
    {
        StopWatch sw;
        std::for_each(task.begin(), task.end(),
                      [this](const Detail::Shard& shard) { metaMgr_.Insert(shard.owner); });
        UC_DEBUG("Fake dump({}) costs {:.3f}ms.", task.size(), sw.Elapsed().count() * 1e3);
        return NextId();
    }
    Expected<bool> Check(Detail::TaskHandle taskId) override { return true; }
    Status Wait(Detail::TaskHandle taskId) override { return Status::OK(); }

private:
    static Detail::TaskHandle NextId() noexcept
    {
        static std::atomic<Detail::TaskHandle> idSeed{1};
        return idSeed.fetch_add(1, std::memory_order_relaxed);
    };
    Status CheckConfig(const Config& config)
    {
        if (config.uniqueId.empty()) { return Status::InvalidParam("invalid unique id"); }
        if (config.bufferNumber < 1024) {
            return Status::InvalidParam("too small buffer number({})", config.bufferNumber);
        }
        if (!config.shareBufferEnable) { return Status::InvalidParam("buffer must be shared"); }
        return Status::OK();
    }
    void ShowConfig(const Config& config)
    {
        const auto& ns = Readme();
        std::string buildType = UCM_BUILD_TYPE;
        if (buildType.empty()) { buildType = "Release"; }
        UC_INFO("{}-{}({}).", ns, UCM_COMMIT_ID, buildType);
        UC_INFO("Set {}::UniqueId to {}.", ns, config.uniqueId);
        UC_INFO("Set {}::BufferNumber to {}.", ns, config.bufferNumber);
        UC_INFO("Set {}::ShareBufferEnable to {}.", ns, config.shareBufferEnable);
    }
};

}  // namespace UC::FakeStore

extern "C" UC::StoreV1* MakeFakeStore() { return new UC::FakeStore::FakeStore(); }
