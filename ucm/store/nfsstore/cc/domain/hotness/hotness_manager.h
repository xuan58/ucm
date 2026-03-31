namespace UC {

class HotnessManager {
public:
    Status Setup(const size_t interval, const SpaceLayout* spaceLayout)
    {
        this->hotnessTimer_.SetInterval(interval);
        this->layout_ = spaceLayout;
        this->setupSuccess_ = true;
        return Status::OK();
    }
    
    void Visit(const std::string& blockId)
    {
        if (!this->setupSuccess_) {
            return;
        }

        this->hotnessSet_.Insert(blockId);
        auto old = this->serviceRunning_.load(std::memory_order_acquire);
        if (old) { return; }
        if (this->serviceRunning_.compare_exchange_weak(old, true, std::memory_order_acq_rel)) {
            auto updater = std::bind(&HotnessSet::UpdateHotness, &this->hotnessSet_, this->layout_);
            if (this->hotnessTimer_.Start(std::move(updater)).Success()) {
                UC_INFO("Space hotness service started.");
                return;
            }
            this->serviceRunning_ = old;
        }
    }

private:
    bool setupSuccess_{false};
    std::atomic_bool serviceRunning_{false};
    const SpaceLayout* layout_;
    HotnessSet hotnessSet_;
    HotnessTimer hotnessTimer_;
};

} // namespace UC

#endif