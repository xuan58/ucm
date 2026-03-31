namespace UC::Trans {

std::shared_ptr<void> CudaBuffer::MakeDeviceBuffer(size_t size)
{
    void* device = nullptr;
    auto ret = cudaMalloc(&device, size);
    if (ret == cudaSuccess) { return std::shared_ptr<void>(device, cudaFree); }
    return nullptr;
}

std::shared_ptr<void> CudaBuffer::MakeHostBuffer(size_t size)
{
    void* host = nullptr;
    auto ret = cudaMallocHost(&host, size);
    if (ret == cudaSuccess) { return std::shared_ptr<void>(host, cudaFreeHost); }
    return nullptr;
}

Status Buffer::RegisterHostBuffer(void* host, size_t size, void** pDevice)
{
    auto ret = cudaHostRegister(host, size, cudaHostRegisterDefault);
    if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    if (pDevice) {
        ret = cudaHostGetDevicePointer(pDevice, host, 0);
        if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    }
    return Status::OK();
}

void Buffer::UnregisterHostBuffer(void* host) { cudaHostUnregister(host); }

} // namespace UC::Trans
