namespace UC::Ds3fsStore {

class Ds3fsFile {
public:
    struct AccessMode {
        static constexpr int32_t READ = R_OK;
        static constexpr int32_t WRITE = W_OK;
        static constexpr int32_t EXIST = F_OK;
        static constexpr int32_t EXECUTE = X_OK;
    };
    struct OpenFlag {
        static constexpr uint32_t READ_ONLY = O_RDONLY;
        static constexpr uint32_t WRITE_ONLY = O_WRONLY;
        static constexpr uint32_t READ_WRITE = O_RDWR;
        static constexpr uint32_t CREATE = O_CREAT;
        static constexpr uint32_t DIRECT = O_DIRECT;
        static constexpr uint32_t APPEND = O_APPEND;
        static constexpr uint32_t EXCL = O_EXCL;
    };

private:
    std::string path_{};
    int32_t handle_{-1};

public:
    explicit Ds3fsFile(std::string path) : path_{std::move(path)} {}
    ~Ds3fsFile();
    const std::string& Path() const { return path_; }
    int32_t ReleaseHandle()
    {
        int32_t fd = handle_;
        handle_ = -1;
        return fd;
    }
    Status MkDir();
    Status RmDir();
    Status Rename(const std::string& newName);
    Status Access(const int32_t mode);
    Status Open(const uint32_t flags);
    void Close();
    void Remove();
};

}  // namespace UC::Ds3fsStore

#endif
