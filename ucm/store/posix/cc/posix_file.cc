namespace UC::PosixStore {

static constexpr auto NewFilePerm = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
static constexpr auto NewDirPerm = (S_IRWXU | S_IRWXG | S_IROTH);

PosixFile::~PosixFile()
{
    if (handle_ != -1) { Close(); }
}

Status PosixFile::MkDir()
{
    const auto dir = path_.c_str();
    auto ret = mkdir(dir, NewDirPerm);
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == EEXIST) { return Status::DuplicateKey(); }
        return Status::OsApiError(std::to_string(eno));
    }
    chmod(dir, NewDirPerm);
    return Status::OK();
}

Status PosixFile::RmDir()
{
    auto ret = rmdir(path_.c_str());
    auto eno = errno;
    if (ret != 0) [[unlikely]] { return Status::OsApiError(std::to_string(eno)); }
    return Status::OK();
}

Status PosixFile::Rename(const std::string& newName)
{
    auto ret = rename(path_.c_str(), newName.c_str());
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == ENOENT) { return Status::NotFound(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status PosixFile::Access(const int32_t mode)
{
    auto ret = access(path_.c_str(), mode);
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == ENOENT) { return Status::NotFound(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status PosixFile::Open(const uint32_t flags)
{
    handle_ = open(path_.c_str(), flags, NewFilePerm);
    auto eno = errno;
    if (handle_ < 0) [[unlikely]] {
        if (eno == EEXIST) { return Status::DuplicateKey(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

void PosixFile::Close()
{
    close(handle_);
    handle_ = -1;
}

void PosixFile::Remove() { remove(path_.c_str()); }

Status PosixFile::Read(void* buffer, size_t size, off64_t offset)
{
    ssize_t nBytes = -1;
    if (offset != -1) {
        nBytes = pread(handle_, buffer, size, offset);
    } else {
        nBytes = read(handle_, buffer, size);
    }
    auto eno = errno;
    if (nBytes != static_cast<ssize_t>(size)) [[unlikely]] {
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status PosixFile::Write(const void* buffer, size_t size, off64_t offset)
{
    ssize_t nBytes = -1;
    if (offset != -1) {
        nBytes = pwrite(handle_, buffer, size, offset);
    } else {
        nBytes = write(handle_, buffer, size);
    }
    auto eno = errno;
    if (nBytes != static_cast<ssize_t>(size)) [[unlikely]] {
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

}  // namespace UC::PosixStore
