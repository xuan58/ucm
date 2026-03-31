namespace UC::PosixStore {

class IoEnginePsync : public Detail::TaskWrapper<TransTask, Detail::TaskHandle> {
    TransQueue queue_;
    size_t shardSize_;

public:
    Status Setup(const Config& config, const SpaceLayout* layout)
    {
        timeoutMs_ = config.timeoutMs;
        shardSize_ = config.shardSize;
        return queue_.Setup(config, &failureSet_, layout);
    }

protected:
    void Dispatch(TaskPtr t, WaiterPtr w) override
    {
        const auto id = t->id;
        const auto& brief = t->desc.brief;
        const auto num = t->desc.size();
        const auto size = shardSize_ * num;
        const auto tp = w->startTp;
        UC_DEBUG("Posix task({},{},{},{}) dispatching.", id, brief, num, size);
        w->SetEpilog([id, brief = std::move(brief), num, size, tp] {
            auto cost = NowTime::Now() - tp;
            UC_DEBUG("Posix task({},{},{},{}) finished, cost {:.3f}ms.", id, brief, num, size,
                     cost * 1e3);
        });
        queue_.Push(t, w);
    }
    void Cancel(TaskPtr t) override { queue_.Cancel(t); }
};

}  // namespace UC::PosixStore

#endif
