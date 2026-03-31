namespace UC::Ds3fsStore {

class SpaceLayout {
public:
    Status Setup(const std::string& storageBackend);
    std::string DataFilePath(const Detail::BlockId& blockId, bool activated) const;
    Status CommitFile(const Detail::BlockId& blockId, bool success) const;

private:
    std::vector<std::string> RelativeRoots() const;
    std::string DataParentName(const std::string& blockFile, bool activated) const;
    std::string DataFileRoot() const;
    std::string TempFileRoot() const;
    std::string DataFileName(const Detail::BlockId& blockId) const;

private:
    std::string storageBackend_;
};

}  // namespace UC::Ds3fsStore

#endif