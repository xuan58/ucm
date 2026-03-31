namespace UC::Ds3fsStore {

Status SpaceManager::Setup(const Config& config)
{
    return layout_.Setup(config.storageBackends[0]);
}

std::vector<uint8_t> SpaceManager::Lookup(const Detail::BlockId* blocks, size_t num)
{
    std::vector<uint8_t> res(num);
    for (size_t i = 0; i < num; i++) { res[i] = Lookup(blocks + i); }
    return res;
}

uint8_t SpaceManager::Lookup(const Detail::BlockId* block)
{
    const auto& path = layout_.DataFilePath(*block, false);
    Ds3fsFile file(path);
    constexpr auto mode =
        Ds3fsFile::AccessMode::EXIST | Ds3fsFile::AccessMode::READ | Ds3fsFile::AccessMode::WRITE;
    auto s = file.Access(mode);
    if (s.Failure()) {
        if (s != Status::NotFound()) { UC_ERROR("Failed({}) to access file({}).", s, path); }
        return false;
    }
    return true;
}

}  // namespace UC::Ds3fsStore