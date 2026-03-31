namespace UC {

class SpaceLayout {
public:
    struct DataIterator;
public:
    virtual ~SpaceLayout() = default;
    virtual Status Setup(const std::vector<std::string>& storageBackends) = 0;
    virtual std::string DataFileParent(const std::string& blockId, bool activated) const = 0;
    virtual std::string DataFilePath(const std::string& blockId, bool activated) const = 0;
    virtual std::string ClusterPropertyFilePath() const = 0;
    virtual std::shared_ptr<DataIterator> CreateFilePathIterator() const = 0;
    virtual std::string NextDataFilePath(std::shared_ptr<DataIterator> iter) const = 0;
    virtual bool IsActivatedFile(const std::string& filePath) const = 0;
};

} // namespace UC

#endif
