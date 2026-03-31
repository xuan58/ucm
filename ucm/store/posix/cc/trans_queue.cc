namespace UC::PosixStore {

Status TransQueue::Setup(const Config& config, TaskIdSet* failureSet, const SpaceLayout* layout)
{
    failureSet_ = failureSet;
    layout_ = layout;
    ioSize_ = config.tensorSize;
    shardSize_ = config.shardSize;
    nShardPerBlock_ = config.blockSize / config.shardSize;
    ioDirect_ = config.ioDirect;
    timeoutMs_ = config.timeoutMs;
    auto success =
        loadPool_.SetNWorker(config.dataTransConcurrency)
            .SetWorkerFn([this](auto& ios, auto&) { LoadWorker(ios); })
            .SetWorkerTimeoutFn([this](IoUnit& ios, ssize_t tid) { OnIoUnitTimeout(ios); },
                                config.timeoutMs)
            .SetCpuAffinity(config.cpuAffinityCores)
            .Run();
    if (!success) [[unlikely]] {
        return Status::Error(fmt::format("workers({}) start failed", config.dataTransConcurrency));
    }
    success = dumpPool_.SetNWorker(config.dataTransConcurrency)
                  .SetWorkerFn([this](auto& ios, auto&) { DumpWorker(ios); })
                  .SetWorkerTimeoutFn([this](IoUnit& ios, ssize_t tid) { OnIoUnitTimeout(ios); },
                                      config.timeoutMs)
                  .SetCpuAffinity(config.cpuAffinityCores)
                  .Run();
    if (!success) [[unlikely]] {
        return Status::Error(fmt::format("workers({}) start failed", config.dataTransConcurrency));
    }
    return Status::OK();
}

void TransQueue::OnIoUnitTimeout(IoUnit& ios)
{
    if (!failureSet_->Contains(ios.owner)) { failureSet_->Insert(ios.owner); }
    ios.waiter->Done();
}

void TransQueue::Push(TaskPtr task, WaiterPtr waiter)
{
    waiter->Set(task->desc.size());
    std::list<IoUnit> ios;
    for (auto&& shard : task->desc) {
        ios.emplace_back<IoUnit>({task->id, std::move(shard), waiter});
    }
    ios.front().firstIo = true;
    if (task->type == TransTask::Type::DUMP) {
        dumpPool_.Push(ios);
    } else {
        loadPool_.Push(ios);
    }
}

void TransQueue::Cancel(TaskPtr task)
{
    auto& pool = task->type == TransTask::Type::DUMP ? dumpPool_ : loadPool_;
    const auto tid = task->id;
    pool.TraverseWaitQueue(
        [this, tid](IoUnit& ios) { return ios.owner == tid || ios.waiter->IsTimeout(timeoutMs_); },
        [this](IoUnit& ios) { OnIoUnitTimeout(ios); },
        [this, tid](IoUnit& ios) { return ios.owner > tid && !ios.waiter->IsTimeout(timeoutMs_); });
}

void TransQueue::LoadWorker(IoUnit& ios)
{
    if (ios.firstIo) {
        auto wait = NowTime::Now() - ios.waiter->startTp;
        UC_DEBUG("Posix load task({}) start running, wait {:.3f}ms.", ios.owner, wait * 1e3);
    }
    if (failureSet_->Contains(ios.owner)) {
        ios.waiter->Done();
        return;
    }
    auto s = S2H(ios);
    if (s.Failure()) [[unlikely]] { failureSet_->Insert(ios.owner); }
    ios.waiter->Done();
}

void TransQueue::DumpWorker(IoUnit& ios)
{
    if (ios.firstIo) {
        auto wait = NowTime::Now() - ios.waiter->startTp;
        UC_DEBUG("Posix dump task({}) start running, wait {:.3f}ms.", ios.owner, wait * 1e3);
    }
    if (failureSet_->Contains(ios.owner)) {
        ios.waiter->Done();
        return;
    }
    auto s = H2S(ios);
    if (ios.shard.index + 1 == nShardPerBlock_) {
        layout_->CommitFile(ios.shard.owner, s.Success());
    }
    if (s.Failure()) [[unlikely]] { failureSet_->Insert(ios.owner); }
    ios.waiter->Done();
}

Status TransQueue::H2S(IoUnit& ios)
{
    const auto& path = layout_->DataFilePath(ios.shard.owner, true);
    PosixFile file{path};
    auto flags = PosixFile::OpenFlag::CREATE | PosixFile::OpenFlag::WRITE_ONLY;
    if (ioDirect_) { flags |= PosixFile::OpenFlag::DIRECT; }
    auto s = file.Open(flags);
    if (s.Failure()) [[unlikely]] {
        UC_ERROR("Failed({}) to open file({}) with flags({}).", s, path, flags);
        return s;
    }
    auto offset = shardSize_ * ios.shard.index;
    for (const auto& addr : ios.shard.addrs) {
        s = file.Write(addr, ioSize_, offset);
        if (s.Failure()) [[unlikely]] {
            UC_ERROR("Failed({}) to write file({}:{}).", s, path, offset);
            return s;
        }
        offset += ioSize_;
    }
    return Status::OK();
}

Status TransQueue::S2H(IoUnit& ios)
{
    const auto& path = layout_->DataFilePath(ios.shard.owner, false);
    PosixFile file{path};
    auto flags = PosixFile::OpenFlag::READ_ONLY;
    if (ioDirect_) { flags |= PosixFile::OpenFlag::DIRECT; }
    auto s = file.Open(flags);
    if (s.Failure()) [[unlikely]] {
        UC_ERROR("Failed({}) to open file({}) with flags({}).", s, path, flags);
        return s;
    }
    auto offset = shardSize_ * ios.shard.index;
    for (const auto& addr : ios.shard.addrs) {
        s = file.Read(addr, ioSize_, offset);
        if (s.Failure()) [[unlikely]] {
            UC_ERROR("Failed({}) to read file({}:{}).", s, path, offset);
            return s;
        }
        offset += ioSize_;
    }
    return Status::OK();
}

}  // namespace UC::PosixStore
