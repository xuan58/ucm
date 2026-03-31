namespace UC::Ds3fsStore {

class SpaceManager {
    SpaceLayout layout_;

public:
    Status Setup(const Config& config);
    std::vector<uint8_t> Lookup(const Detail::BlockId* blocks, size_t num);
    const SpaceLayout* GetLayout() const { return &layout_; }

private:
    uint8_t Lookup(const Detail::BlockId* block);
};

}  // namespace UC::Ds3fsStore

#endif