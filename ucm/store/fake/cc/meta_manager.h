namespace UC::FakeStore {

class MetaStrategy;

class MetaManager {
    std::shared_ptr<MetaStrategy> strategy_{nullptr};

public:
    Status Setup(const Config& config);
    void Insert(const Detail::BlockId& block) noexcept;
    bool Exist(const Detail::BlockId& block) const noexcept;

private:
    bool ExistAt(size_t iBucket, const Detail::BlockId& block) const noexcept;
    void InsertAt(size_t iBucket, const Detail::BlockId& block) noexcept;
    void MoveTo(size_t iBucket, size_t iNode) noexcept;
    void Remove(size_t iBucket, size_t iNode) noexcept;
};

}  // namespace UC::FakeStore

#endif
