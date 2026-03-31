namespace UC {

class SpaceShardLayout : public SpaceLayout {
public:
    struct DataIterator;
public:
    Status Setup(const std::vector<std::string>& storageBackends) override;
    std::string DataFileParent(const std::string& blockId, bool activated) const override;
    std::string DataFilePath(const std::string& blockId, bool activated) const override;
    std::string ClusterPropertyFilePath() const override;
    std::shared_ptr<SpaceLayout::DataIterator> CreateFilePathIterator() const override;
    std::string NextDataFilePath(std::shared_ptr<SpaceLayout::DataIterator> iter) const override;
    bool IsActivatedFile(const std::string& filePath) const override;

protected:
    virtual std::vector<std::string> RelativeRoots() const;
    virtual Status AddStorageBackend(const std::string& path);
    virtual Status AddFirstStorageBackend(const std::string& path);
    virtual Status AddSecondaryStorageBackend(const std::string& path);
    virtual std::string StorageBackend(const std::string& blockId) const;
    virtual std::string DataFileRoot() const;
    virtual std::string ClusterFileRoot() const;
    virtual std::string StorageBackend() const;
    virtual void ShardBlockId(const std::string& blockId, uint64_t& front, uint64_t& back) const;
    std::vector<std::string> storageBackends_;
};

} // namespace UC

#endif
