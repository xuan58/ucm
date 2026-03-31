namespace UC {

class ShareBuffer {
public:
    class Reader {
        std::string block_;
        std::string path_;
        size_t length_;
        bool ioDirect_;
        bool shared_;
        void* addr_;

    public:
        bool Shared() const noexcept { return shared_; }
        Status Ready4Read();
        uintptr_t GetData();

    private:
        Reader(const std::string& block, const std::string& path, const size_t length,
               const bool ioDirect, const bool shared, void* addr)
            : block_{block},
              path_{path},
              length_{length},
              ioDirect_{ioDirect},
              shared_{shared},
              addr_{addr}
        {
        }
        friend class ShareBuffer;
        Status Ready4ReadOnLocalBuffer();
        Status Ready4ReadOnSharedBuffer();
    };

public:
    Status Setup(const size_t blockSize, const size_t blockNumber, const bool ioDirect,
                 const std::string& uniqueId);
    ~ShareBuffer();
    std::shared_ptr<Reader> MakeReader(const std::string& block, const std::string& path);

private:
    size_t DataOffset() const;
    size_t ShmSize() const;
    Status InitShmBuffer(IFile* file);
    Status LoadShmBuffer(IFile* file);
    size_t AcquireBlock(const std::string& block);
    void ReleaseBlock(const size_t index);
    void* BlockAt(const size_t index);
    std::shared_ptr<Reader> MakeLocalReader(const std::string& block, const std::string& path);
    std::shared_ptr<Reader> MakeSharedReader(const std::string& block, const std::string& path,
                                             size_t position);

private:
    size_t blockSize_;
    size_t blockNumber_;
    bool ioDirect_;
    std::string shmName_;
    void* addr_{nullptr};
    std::unique_ptr<Trans::Buffer> tmpBufMaker_{nullptr};
};

}  // namespace UC

#endif
