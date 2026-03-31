namespace UC::Metrics {
struct MetricBuffer {
    struct InnerBuffer {
        std::unordered_map<std::string, double> counterStats_;
        std::unordered_map<std::string, double> gaugeStats_;
        std::unordered_map<std::string, std::vector<double>> histogramStats_;

        void Clear()
        {
            counterStats_.clear();
            gaugeStats_.clear();
            histogramStats_.clear();
        }

        std::shared_mutex bufferMutex_;
    };

    InnerBuffer innerBufs_[2];
    std::atomic<int> writeIdx_{0};

    int SwitchBuffer()
    {
        int oldIdx = writeIdx_.exchange(1 - writeIdx_.load(std::memory_order_acquire),
                                        std::memory_order_acq_rel);
        return oldIdx;
    }

    InnerBuffer& GetWriteBuffer(int idx) { return innerBufs_[idx]; }

    const InnerBuffer& GetReadBuffer(int idx) const { return innerBufs_[idx]; }

    void ClearReadBuffer(int idx) { innerBufs_[idx].Clear(); }
};

class Metrics {
public:
    static Metrics& GetInstance()
    {
        static Metrics inst;
        return inst;
    }

    void SetUp(size_t maxVectorLen)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (isInited_.load(std::memory_order_acquire)) { return; }
        bool expected = false;
        if (isInited_.compare_exchange_strong(expected, true, std::memory_order_release,
                                              std::memory_order_relaxed)) {
            maxVectorLen_ = maxVectorLen;
        }
    }

    ~Metrics() = default;

    void CreateStats(const std::string& name, const std::string& type);

    void UpdateStats(const std::string& name, double value);

    void UpdateStats(const std::unordered_map<std::string, double>& values);

    std::tuple<std::unordered_map<std::string, double>, std::unordered_map<std::string, double>,
               std::unordered_map<std::string, std::vector<double>>>
    GetAllStatsAndClear();

private:
    enum class MetricType : int { COUNTER = 0, GAUGE = 1, HISTOGRAM = 2 };

    std::shared_mutex mutex_;
    std::unordered_map<std::string, MetricType> statsType_;
    std::list<std::shared_ptr<MetricBuffer>> buffers_;
    static thread_local std::shared_ptr<MetricBuffer> threadBuffer_;
    static thread_local bool isRegisteredThread_;

    Metrics() = default;
    Metrics(const Metrics&) = delete;
    Metrics& operator=(const Metrics&) = delete;
    std::atomic<bool> isInited_{false};
    size_t maxVectorLen_{10000};
};
}  // namespace UC::Metrics

#endif  // UNIFIEDCACHE_METRICS_H