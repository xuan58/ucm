namespace UC {

class SpaceShardTempLayout : public SpaceShardLayout {
public:
    std::string DataFileParent(const std::string& blockId, bool activated) const override;
    std::string DataFilePath(const std::string& blockId, bool activated) const override;

protected:
    std::vector<std::string> RelativeRoots() const override;
    virtual std::string TempDataFileRoot() const;
};

} // namespace UC

#endif
