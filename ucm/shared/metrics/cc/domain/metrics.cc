namespace UC::Metrics {
thread_local std::shared_ptr<MetricBuffer> Metrics::threadBuffer_ =
    std::make_shared<MetricBuffer>();
thread_local bool Metrics::isRegisteredThread_ = false;

void Metrics::CreateStats(const std::string& name, const std::string& type)
{
    if (!isInited_.load(std::memory_order_acquire)) {
        throw std::runtime_error("Please call SetUp() first!");
    }
    std::string typeUpper = type;
    std::transform(typeUpper.begin(), typeUpper.end(), typeUpper.begin(), ::toupper);
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (statsType_.count(name)) {
        return;
    } else {
        if (typeUpper == "COUNTER") {
            statsType_[name] = MetricType::COUNTER;
        } else if (typeUpper == "GAUGE") {
            statsType_[name] = MetricType::GAUGE;
        } else if (typeUpper == "HISTOGRAM") {
            statsType_[name] = MetricType::HISTOGRAM;
        } else {
            return;
        }
    }
}

void Metrics::UpdateStats(const std::string& name, double value)
{
    if (!isRegisteredThread_) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        buffers_.push_back({threadBuffer_});
        isRegisteredThread_ = true;
    }

    auto it = statsType_.find(name);
    if (it == statsType_.end()) { return; }

    int writeIdx_ = threadBuffer_->writeIdx_.load(std::memory_order_acquire);
    std::shared_lock<std::shared_mutex> lock(threadBuffer_->innerBufs_[writeIdx_].bufferMutex_);
    auto& writeBuf = threadBuffer_->GetWriteBuffer(writeIdx_);

    switch (it->second) {
        case MetricType::COUNTER: writeBuf.counterStats_[name] += value; break;
        case MetricType::GAUGE: writeBuf.gaugeStats_[name] = value; break;
        case MetricType::HISTOGRAM:
            if (writeBuf.histogramStats_[name].size() < maxVectorLen_) {
                writeBuf.histogramStats_[name].push_back(value);
            }
            break;

        default: break;
    }
}

void Metrics::UpdateStats(const std::unordered_map<std::string, double>& values)
{
    for (const auto& pair : values) { UpdateStats(pair.first, pair.second); }
}

std::tuple<std::unordered_map<std::string, double>, std::unordered_map<std::string, double>,
           std::unordered_map<std::string, std::vector<double>>>
Metrics::GetAllStatsAndClear()
{
    std::unordered_map<std::string, double> totalCounter;
    std::unordered_map<std::string, double> totalGauge;
    std::unordered_map<std::string, std::vector<double>> totalHistogram;

    for (const auto& buf : buffers_) {
        int oldIdx = buf->SwitchBuffer();
        std::unique_lock<std::shared_mutex> lock(buf->innerBufs_[oldIdx].bufferMutex_);
        auto& read_buf = buf->GetReadBuffer(oldIdx);

        for (const auto& [name, value] : read_buf.counterStats_) { totalCounter[name] += value; }

        for (const auto& [name, value] : read_buf.gaugeStats_) { totalGauge[name] = value; }

        for (auto& [name, values] : read_buf.histogramStats_) {
            totalHistogram[name].insert(totalHistogram[name].end(), values.begin(), values.end());
        }
        buf->ClearReadBuffer(oldIdx);
    }

    auto result =
        std::make_tuple(std::move(totalCounter), std::move(totalGauge), std::move(totalHistogram));

    return result;
}

}  // namespace UC::Metrics