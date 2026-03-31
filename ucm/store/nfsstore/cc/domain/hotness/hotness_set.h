namespace UC {

class HotnessSet {
public:
    void Insert(const std::string& blockId);
    void UpdateHotness(const SpaceLayout* spaceLayout);

private:
    std::mutex mutex_;
    std::unordered_set<std::string> pendingBlocks_;
};


} // namespace UC

#endif