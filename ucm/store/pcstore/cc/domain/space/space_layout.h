namespace UC {

class SpaceLayout {
public:
    Status Setup(const std::vector<std::string>& storageBackends, bool shardDataDir);
    std::string DataFilePath(const std::string& blockId, bool activated) const;
    Status Commit(const std::string& blockId, bool success) const;

private:
    std::vector<std::string> RelativeRoots() const;
    Status AddStorageBackend(const std::string& path);
    Status AddFirstStorageBackend(const std::string& path);
    Status AddSecondaryStorageBackend(const std::string& path);
    std::string StorageBackend(const std::string& blockId) const;
    std::string DataParentName(const std::string& blockFile, bool activated) const;
    std::string DataFileRoot() const;
    std::string TempFileRoot() const;
    std::string DataFileName(const std::string& blockId) const;

private:
    std::vector<std::string> storageBackends_;
    bool shardDataDir_;
};

}  // namespace UC

#endif
