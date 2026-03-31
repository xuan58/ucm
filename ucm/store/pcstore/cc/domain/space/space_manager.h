namespace UC {

class SpaceManager {
public:
    Status Setup(const std::vector<std::string>& storageBackends, const size_t blockSize,
                 bool shardDataDir);
    Status NewBlock(const std::string& blockId);
    Status CommitBlock(const std::string& blockId, bool success);
    bool LookupBlock(const std::string& blockId) const;
    const SpaceLayout* GetSpaceLayout() const { return &this->layout_; }

private:
    SpaceLayout layout_;
    size_t blockSize_;
};

} // namespace UC

#endif
