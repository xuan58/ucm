namespace UC::Trans {

Status Device::Setup(int32_t deviceId)
{
    auto ret = cudaSetDevice(deviceId);
    if (ret != cudaSuccess) { return Status{ret, cudaGetErrorString(ret)}; }
    return Status::OK();
}

std::unique_ptr<Stream> Device::MakeStream()
{
    std::unique_ptr<Stream> stream = nullptr;
    try {
        stream = std::make_unique<CudaStream>();
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
        stream = std::make_shared<CudaStream>();
    } catch (...) {
        return nullptr;
    }
    if (stream->Setup().Success()) { return stream; }
    return nullptr;
}

std::unique_ptr<Stream> Device::MakeSMStream()
{
    std::unique_ptr<Stream> stream = nullptr;
    try {
        stream = std::make_unique<CudaSmStream>();
    } catch (...) {
        return nullptr;
    }
    if (stream->Setup().Success()) { return stream; }
    return nullptr;
}

std::unique_ptr<Buffer> Device::MakeBuffer()
{
    try {
        return std::make_unique<CudaBuffer>();
    } catch (...) {
        return nullptr;
    }
}

} // namespace UC::Trans
