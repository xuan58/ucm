namespace UC::PosixStore {

class HotnessTracker {
public:
    HotnessTracker() = default;
    HotnessTracker(const HotnessTracker&) = delete;
    HotnessTracker& operator=(const HotnessTracker&) = delete;
    ~HotnessTracker();
    Status Setup(const SpaceLayout* layout);
    void Touch(const Detail::BlockId& blockId);

private:
    void UtimeWorkerLoop();
    const SpaceLayout* layout_{nullptr};
    std::deque<Detail::BlockId> produceQueue_;
    std::mutex queueMtx_;
    std::atomic<bool> stop_{false};
    std::thread utimeWorker_;
};

}  // namespace UC::PosixStore

#endif
