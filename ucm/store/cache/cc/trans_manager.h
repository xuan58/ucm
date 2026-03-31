namespace UC::CacheStore {

class TransManager : public Detail::TaskWrapper<TransTask, Detail::TaskHandle> {
    size_t shardSize_;
    LoadQueue loadQ_;
    DumpQueue dumpQ_;

public:
    Status Setup(const Config& config, TransBuffer* buffer)
    {
        timeoutMs_ = config.timeoutMs;
        shardSize_ = config.shardSize;
        auto s = loadQ_.Setup(config, &failureSet_, buffer);
        if (s.Failure()) [[unlikely]] { return s; }
        return dumpQ_.Setup(config, &failureSet_, buffer);
    }

protected:
    void Dispatch(TaskPtr t, WaiterPtr w) override
    {
        const auto id = t->id;
        const auto& brief = t->desc.brief;
        const auto num = t->desc.size();
        const auto size = shardSize_ * num;
        const auto tp = w->startTp;
        UC_DEBUG("Cache task({},{},{},{}) dispatching.", id, brief, num, size);
        w->SetEpilog([id, brief = std::move(brief), num, size, tp] {
            auto cost = NowTime::Now() - tp;
            UC_DEBUG("Cache task({},{},{},{}) finished, cost {:.3f}ms.", id, brief, num, size,
                     cost * 1e3);
        });
        if (t->type == TransTask::Type::LOAD) {
            loadQ_.Submit(t, w);
        } else {
            dumpQ_.Submit(t, w);
        }
    }
};

}  // namespace UC::CacheStore

#endif
