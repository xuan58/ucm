namespace UC {

void HotnessSet::Insert(const std::string& blockId)
{
    std::lock_guard<std::mutex> lg(this->mutex_);
    this->pendingBlocks_.insert(blockId);
}

void HotnessSet::UpdateHotness(const SpaceLayout* spaceLayout)
{
    std::unordered_set<std::string> blocksToUpdate;
    {
        std::lock_guard<std::mutex> lg(this->mutex_);
        if (this->pendingBlocks_.empty()) {
            return;
        }
        blocksToUpdate.swap(this->pendingBlocks_);
    }

    size_t number = 0;
    for (const std::string& blockId : blocksToUpdate) {
        auto blockPath = spaceLayout->DataFilePath(blockId, false);
        auto file = File::Make(blockPath);
        if (!file) {
            UC_WARN("Failed to make file({}), blockId({}).", blockPath, blockId);
            continue;
        }
        auto status = file->UpdateTime();
        if (status.Failure()) {
            UC_WARN("Failed({}) to update time({}), blockId({}).", status, blockPath, blockId);
            continue;
        }
        number++;
    }
    if (blocksToUpdate.size() == number) {
        UC_INFO("All blocks are hotness.");
    } else {
        UC_WARN("{} of {} blocks are hotness.", blocksToUpdate.size() - number, blocksToUpdate.size());
    }
}

} // namespace UC