namespace UC::Ds3fsStore {

Status SpaceLayout::Setup(const std::string& storageBackend)
{
    auto normalizedPath = storageBackend;
    if (normalizedPath.back() != '/') { normalizedPath += '/'; }

    for (const auto& root : this->RelativeRoots()) {
        Ds3fsFile dir(normalizedPath + root);
        auto status = dir.MkDir();
        if (status == Status::DuplicateKey()) { status = Status::OK(); }
        if (status.Failure()) { return status; }
    }

    this->storageBackend_ = normalizedPath;
    return Status::OK();
}

std::string SpaceLayout::DataFilePath(const Detail::BlockId& blockId, bool activated) const
{
    const auto& file = DataFileName(blockId);
    const auto& parent = DataParentName(file, activated);
    return fmt::format("{}{}/{}", storageBackend_, parent, file);
}

Status SpaceLayout::CommitFile(const Detail::BlockId& blockId, bool success) const
{
    const auto& file = DataFileName(blockId);
    const auto& activated = fmt::format("{}{}/{}", storageBackend_, TempFileRoot(), file);
    auto s = Status::OK();
    if (success) {
        const auto& parent = fmt::format("{}{}", storageBackend_, DataParentName(file, false));
        const auto& archived = fmt::format("{}/{}", parent, file);
        Ds3fsFile dir(parent);
        s = dir.MkDir();
        if (s == Status::OK() || s == Status::DuplicateKey()) {
            Ds3fsFile activatedFile(activated);
            s = activatedFile.Rename(archived);
        }
    }
    if (!success || s.Failure()) {
        Ds3fsFile activatedFile(activated);
        activatedFile.Remove();
    }
    return s;
}

std::vector<std::string> SpaceLayout::RelativeRoots() const { return {TempFileRoot()}; }

std::string SpaceLayout::DataParentName(const std::string& blockFile, bool activated) const
{
    if (activated) { return TempFileRoot(); }
    return blockFile.substr(0, 2);
}

std::string SpaceLayout::DataFileRoot() const { return "data"; }

std::string SpaceLayout::TempFileRoot() const { return ".temp"; }

std::string SpaceLayout::DataFileName(const Detail::BlockId& blockId) const
{
    return fmt::format("{:02x}", fmt::join(blockId, ""));
}

}  // namespace UC::Ds3fsStore