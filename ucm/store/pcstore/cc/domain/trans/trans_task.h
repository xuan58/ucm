namespace UC {

class TransTask {
    static size_t NextId() noexcept
    {
        static std::atomic<size_t> idSeed{invalid + 1};
        return idSeed.fetch_add(1, std::memory_order_relaxed);
    };
    static double NowTp() noexcept
    {
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration<double>(now).count();
    }

public:
    enum class Type { DUMP, LOAD };
    size_t id;
    Type type;
    double startTp{0};
    static constexpr auto invalid = std::numeric_limits<size_t>::min();
    TransTask(Type&& type, std::string&& brief)
        : id{NextId()}, type{std::move(type)}, startTp{NowTp()}, brief_{std::move(brief)}
    {
    }
    void Append(const std::string& block, const uintptr_t address)
    {
        grouped_[block].push_back(address);
        number_++;
    }
    auto Str() const noexcept { return fmt::format("{},{},{}", id, brief_, number_); }
    size_t GroupNumber() const { return grouped_.size(); }
    void ForEachGroup(std::function<void(const std::string&, std::vector<uintptr_t>&)> fn)
    {
        for (auto& [block, shards] : grouped_) { fn(block, shards); }
    }
    auto Epilog(const size_t ioSize) const noexcept
    {
        auto total = ioSize * number_;
        auto costs = NowTp() - startTp;
        auto bw = double(total) / costs / 1e9;
        return fmt::format("Task({},{},{},{}) finished, costs={:.06f}s, bw={:.06f}GB/s.", id,
                           brief_, number_, total, costs, bw);
    }

private:
    std::string brief_;
    size_t number_{0};
    std::unordered_map<std::string, std::vector<uintptr_t>> grouped_;
};

} // namespace UC

#endif
