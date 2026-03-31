namespace UC::Ds3fsStore {

static constexpr auto NewFilePerm = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
static constexpr auto NewDirPerm = (S_IRWXU | S_IRGRP | S_IROTH);

Ds3fsFile::~Ds3fsFile()
{
    if (handle_ != -1) { Close(); }
}

Status Ds3fsFile::MkDir()
{
    auto ret = mkdir(path_.c_str(), NewDirPerm);
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == EEXIST) { return Status::DuplicateKey(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status Ds3fsFile::RmDir()
{
    auto ret = rmdir(path_.c_str());
    auto eno = errno;
    if (ret != 0) [[unlikely]] { return Status::OsApiError(std::to_string(eno)); }
    return Status::OK();
}

Status Ds3fsFile::Rename(const std::string& newName)
{
    auto ret = rename(path_.c_str(), newName.c_str());
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == ENOENT) { return Status::NotFound(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status Ds3fsFile::Access(const int32_t mode)
{
    auto ret = access(path_.c_str(), mode);
    auto eno = errno;
    if (ret != 0) [[unlikely]] {
        if (eno == ENOENT) { return Status::NotFound(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

Status Ds3fsFile::Open(const uint32_t flags)
{
    handle_ = open(path_.c_str(), flags, NewFilePerm);
    auto eno = errno;
    if (handle_ < 0) [[unlikely]] {
        if (eno == EEXIST) { return Status::DuplicateKey(); }
        return Status::OsApiError(std::to_string(eno));
    }
    return Status::OK();
}

void Ds3fsFile::Close()
{
    close(handle_);
    handle_ = -1;
}

void Ds3fsFile::Remove() { remove(path_.c_str()); }

}  // namespace UC::Ds3fsStore
