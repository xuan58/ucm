namespace UC::PosixStore {

class SpaceLayout {
private:
    std::vector<std::string> storageBackends_;
    std::vector<std::string> shards_;
    bool dataDirShard_;
    size_t dataDirShardBytes_;

public:
    Status Setup(const Config& config);
    std::string DataFilePath(const Detail::BlockId& blockId, bool activated) const;
    Status CommitFile(const Detail::BlockId& blockId, bool success) const;
    Status RemoveFile(const Detail::BlockId& blockId) const;
    std::vector<std::string> SampleShards(double sampleRatio) const;
    size_t CountFilesInShard(const std::string& shard) const;
    std::vector<Detail::BlockId> GetOldestFiles(const std::string& shard, double recyclePercent,
                                                size_t maxRecycleCount) const;

private:
    std::vector<std::string> RelativeRoots() const;
    Status AddStorageBackend(const std::string& path);
    Status AddFirstStorageBackend(const std::string& path);
    Status AddSecondaryStorageBackend(const std::string& path);
    std::string StorageBackend(const Detail::BlockId& blockId) const;
    std::string FileShardName(const std::string& fileName) const
    {
        return fileName.substr(0, dataDirShardBytes_);
    }
};

}  // namespace UC::PosixStore

#endif
