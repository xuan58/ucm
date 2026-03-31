namespace UC {

std::string SpaceShardTempLayout::DataFileParent(const std::string& blockId, bool activated) const
{
    if (!activated) { return SpaceShardLayout::DataFileParent(blockId, activated); }
    uint64_t front, back;
    this->ShardBlockId(blockId, front, back);
    return fmt::format("{}{}/{:016x}", this->StorageBackend(blockId), this->TempDataFileRoot(),
                       front);
}

std::string SpaceShardTempLayout::DataFilePath(const std::string& blockId, bool activated) const
{
    if (!activated) { return SpaceShardLayout::DataFilePath(blockId, activated); }
    uint64_t front, back;
    this->ShardBlockId(blockId, front, back);
    return fmt::format("{}{}/{:016x}/{:016x}.dat", this->StorageBackend(blockId),
                       this->TempDataFileRoot(), front, back);
}

std::vector<std::string> SpaceShardTempLayout::RelativeRoots() const
{
    auto roots = SpaceShardLayout::RelativeRoots();
    roots.push_back(this->TempDataFileRoot());
    return roots;
}

std::string SpaceShardTempLayout::TempDataFileRoot() const { return "temp"; }

} // namespace UC
