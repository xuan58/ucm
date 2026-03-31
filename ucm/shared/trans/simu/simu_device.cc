namespace UC::Trans {

Status Device::Setup(int32_t deviceId)
{
    if (deviceId < 0) { return Status::Error(fmt::format("invalid device id({})", deviceId)); }
    return Status::OK();
}

std::unique_ptr<Stream> Device::MakeStream()
{
    std::unique_ptr<Stream> stream = nullptr;
    try {
        stream = std::make_unique<SimuStream>();
    } catch (...) {
        return nullptr;
    }
    if (stream->Setup().Success()) { return stream; }
    return nullptr;
}

std::shared_ptr<Stream> Device::MakeSharedStream()
{
    std::shared_ptr<Stream> stream = nullptr;
    try {
        stream = std::make_shared<SimuStream>();
    } catch (...) {
        return nullptr;
    }
    if (stream->Setup().Success()) { return stream; }
    return nullptr;
}

std::unique_ptr<Stream> Device::MakeSMStream() { return MakeStream(); }

std::unique_ptr<Buffer> Device::MakeBuffer()
{
    try {
        return std::make_unique<SimuBuffer>();
    } catch (...) {
        return nullptr;
    }
}

}  // namespace UC::Trans
