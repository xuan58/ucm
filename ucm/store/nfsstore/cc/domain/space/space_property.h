namespace UC {

class SpaceProperty {
public:
    ~SpaceProperty();
    Status Setup(const std::string& propertyFilePath);
    void IncreaseCapacity(const size_t delta);
    void DecreaseCapacity(const size_t delta);
    size_t GetCapacity() const;

private:
    Status InitShmProperty(IFile* shmPropertyFile);
    Status LoadShmProperty(IFile* shmPropertyFile);

private:
    void* addr_{nullptr};
};

} // namespace UC

#endif