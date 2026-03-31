namespace UC {

Status SpaceManager::Setup(const std::vector<std::string>& storageBackends, const size_t blockSize,
                           bool shardDataDir)
{
    auto status = this->layout_.Setup(storageBackends, shardDataDir);
    if (status.Failure()) { return status; }
    this->blockSize_ = blockSize;
    return Status::OK();
}

Status SpaceManager::NewBlock(const std::string& blockId)
{
    const auto& activated = this->layout_.DataFilePath(blockId, true);
    const auto& archived = this->layout_.DataFilePath(blockId, false);
    if (File::Access(archived, IFile::AccessMode::EXIST).Success()) {
        return Status::DuplicateKey();
    }
    auto file = File::Make(activated);
    if (!file) { return Status::OutOfMemory(); }
    auto mode = IFile::OpenFlag::CREATE | IFile::OpenFlag::EXCL | IFile::OpenFlag::READ_WRITE;
    auto s = file->Open(mode);
    if (s.Failure()) {
        if (s != Status::DuplicateKey()) { return s; }
        mode = IFile::OpenFlag::READ_WRITE;
        if ((s = file->Open(mode)).Failure()) { return s; }
        IFile::FileStat st;
        if ((s = file->Stat(st)).Failure()) { return s; }
        const auto now = std::chrono::system_clock::now();
        const auto mtime = std::chrono::system_clock::from_time_t(st.st_mtime);
        constexpr auto reuseBlockAge = std::chrono::seconds(300);
        if (now - mtime <= reuseBlockAge) { return Status::DuplicateKey(); }
    }
    return file->Truncate(this->blockSize_);
}

Status SpaceManager::CommitBlock(const std::string& blockId, bool success)
{
    return this->layout_.Commit(blockId, success);
}

bool SpaceManager::LookupBlock(const std::string& blockId) const
{
    const auto& path = this->layout_.DataFilePath(blockId, false);
    constexpr auto mode =
        IFile::AccessMode::EXIST | IFile::AccessMode::READ | IFile::AccessMode::WRITE;
    auto s = File::Access(path, mode);
    if (s.Failure()) {
        if (s != Status::NotFound()) { UC_ERROR("Failed({}) to access file({}).", s, path); }
        return false;
    }
    return true;
}

} // namespace UC
