namespace UC {

class SpaceRecycle {
public:
    using RecycleOneBlockDone = std::function<void(void)>;
    SpaceRecycle() = default;
    SpaceRecycle(const SpaceRecycle&) = delete;
    SpaceRecycle& operator=(const SpaceRecycle&) = delete;
    ~SpaceRecycle();
    Status Setup(const SpaceLayout* layout, const size_t totalNumber,
                 RecycleOneBlockDone done);
    void Trigger();
private:
    void Recycler();
private:
    bool stop_{false};
    bool recycling_{false};
    std::atomic_bool serviceRunning_{false};
    uint32_t recycleNum_{0};
    RecycleOneBlockDone recycleOneBlockDone_;
    const SpaceLayout* layout_{nullptr};
    std::mutex mtx_;
    std::condition_variable cv_;
    std::thread worker_;
};

} // namespace UC
#endif