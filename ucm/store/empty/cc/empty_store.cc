namespace UC::EmptyStore {

std::vector<uint8_t> OnLookup(size_t num) { return std::vector<uint8_t>(num, false); }

class EmptyStore : public StoreV1 {
public:
    Status Setup(const Detail::Dictionary& config) { return Status::OK(); }
    std::string Readme() const { return "EmptyStore"; }
    Expected<std::vector<uint8_t>> Lookup(const Detail::BlockId* blocks, size_t num)
    {
        return OnLookup(num);
    }
    Expected<ssize_t> LookupOnPrefix(const Detail::BlockId* blocks, size_t num) { return -1; }
    void Prefetch(const Detail::BlockId* blocks, size_t num) {}
    Expected<Detail::TaskHandle> Load(Detail::TaskDesc task) { return NextId(); }
    Expected<Detail::TaskHandle> Dump(Detail::TaskDesc task) { return NextId(); }
    Expected<bool> Check(Detail::TaskHandle taskId) { return true; }
    Status Wait(Detail::TaskHandle taskId) { return Status::OK(); }

private:
    static Detail::TaskHandle NextId() noexcept
    {
        static std::atomic<Detail::TaskHandle> id{1};
        return id.fetch_add(1, std::memory_order_relaxed);
    };
};

}  // namespace UC::EmptyStore

extern "C" UC::StoreV1* MakeEmptyStore() { return new UC::EmptyStore::EmptyStore(); }
