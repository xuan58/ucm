namespace UC::PosixStore {

HotnessTracker::~HotnessTracker()
{
    stop_.store(true);
    if (utimeWorker_.joinable()) { utimeWorker_.join(); }
}

Status HotnessTracker::Setup(const SpaceLayout* layout)
{
    layout_ = layout;
    try {
        utimeWorker_ = std::thread(&HotnessTracker::UtimeWorkerLoop, this);
    } catch (const std::exception& e) {
        UC_ERROR("Failed({}) to create utime worker thread.", e.what());
        return Status::OutOfMemory();
    }
    return Status::OK();
}

void HotnessTracker::Touch(const Detail::BlockId& blockId)
{
    std::lock_guard<std::mutex> lock(queueMtx_);
    produceQueue_.push_back(blockId);
}

void HotnessTracker::UtimeWorkerLoop()
{
    std::deque<Detail::BlockId> consumeQueue;
    constexpr size_t kSpinLimit = 16;
    size_t spinCount = 0;
    while (!stop_.load()) {
        {
            std::lock_guard<std::mutex> lock(queueMtx_);
            consumeQueue.swap(produceQueue_);
        }
        if (consumeQueue.empty()) {
            if (++spinCount < kSpinLimit) {
                std::this_thread::yield();
            } else {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                spinCount = 0;
            }
            continue;
        }
        spinCount = 0;
        while (!consumeQueue.empty()) {
            auto filePath = layout_->DataFilePath(consumeQueue.front(), false);
            utime(filePath.c_str(), nullptr);
            consumeQueue.pop_front();
        }
    }
}

}  // namespace UC::PosixStore
