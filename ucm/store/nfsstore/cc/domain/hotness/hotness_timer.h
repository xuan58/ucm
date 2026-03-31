namespace UC {

class HotnessTimer {
public:
    void SetInterval(const size_t interval) { this->interval_ = std::chrono::seconds(interval); }
    Status Start(std::function<void()> callable)
    {
        try {
            this->timer_ = std::make_unique<Timer<std::function<void()>>>(this->interval_,
                                                                          std::move(callable));
        } catch (const std::exception& e) {
            UC_ERROR("Failed({}) to start hotness timer.", e.what());
            return Status::OutOfMemory();
        }
        return this->timer_->Start() ? Status::OK() : Status::Error();
    }

private:
    std::chrono::seconds interval_;
    std::unique_ptr<Timer<std::function<void()>>> timer_;
};

} // namespace UC

#endif
