namespace UC {

class SpaceManager {
public:
    Status Setup(const std::vector<std::string>& storageBackends, const size_t blockSize,
                 const bool tempDumpDirEnable, const size_t storageCapacity = 0,
                 const bool recycleEnable = false, const float recycleThresholdRatio = 0.7f);
    Status NewBlock(const std::string& blockId);
    Status CommitBlock(const std::string& blockId, bool success = true);
    bool LookupBlock(const std::string& blockId) const;
    const SpaceLayout* GetSpaceLayout() const;

private:
    Status CapacityCheck();
private:
    std::unique_ptr<SpaceLayout> layout_;
    SpaceProperty property_;
    SpaceRecycle recycle_;
    size_t blockSize_;
    size_t capacity_;
    bool recycleEnable_;
    size_t capacityRecycleThreshold_;
};

} // namespace UC

#endif
