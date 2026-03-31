namespace UC::CacheStore {

class PosixShm {
public:
    struct OpenFlag {
        static constexpr uint32_t READ_WRITE = O_RDWR;
        static constexpr uint32_t CREATE = O_CREAT;
        static constexpr uint32_t EXCL = O_EXCL;
    };

private:
    std::string name_{};
    int32_t handle_{-1};

public:
    PosixShm(std::string name) : name_{std::move(name)} {}
    ~PosixShm()
    {
        if (handle_ != -1) { close(handle_); }
    }
    const std::string& ShmName() { return name_; }
    Status ShmOpen(const uint32_t flags)
    {
        static constexpr auto NewFilePerm = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        handle_ = shm_open(name_.c_str(), flags, NewFilePerm);
        auto eno = errno;
        if (handle_ >= 0) { return Status::OK(); }
        if (eno == EEXIST) { return Status::DuplicateKey(); }
        return Status{eno, std::to_string(eno)};
    }
    Status Truncate(size_t length)
    {
        auto ret = ftruncate64(handle_, length);
        auto eno = errno;
        if (ret == 0) { return Status::OK(); }
        return Status{eno, std::to_string(eno)};
    }
    Status MMap(void*& addr, size_t size, bool write, bool read, bool shared)
    {
        auto prot = PROT_NONE;
        if (write) { prot |= PROT_WRITE; }
        if (read) { prot |= PROT_READ; }
        auto flags = 0;
        if (shared) { flags |= MAP_SHARED; }
        addr = mmap(nullptr, size, prot, flags, handle_, 0);
        auto eno = errno;
        if (addr != MAP_FAILED) { return Status::OK(); }
        return Status{eno, std::to_string(eno)};
    }
    static void MUnmap(void* addr, size_t size) { munmap(addr, size); }
    void ShmUnlink() { shm_unlink(name_.c_str()); }
};

}  // namespace UC::CacheStore

#endif
