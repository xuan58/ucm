namespace UC::Trans {

Status CudaSmStream::DeviceToHostAsync(void* device[], void* host[], size_t size, size_t number)
{
    auto ret = CudaSMCopyAsync(device, host, size, number, stream_);
    if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    return Status::OK();
}

Status CudaSmStream::DeviceToHostAsync(void* device[], void* host, size_t size, size_t number)
{
    auto ret = CudaSMCopyAsync(device, host, size, number, stream_);
    if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    return Status::OK();
}

Status CudaSmStream::HostToDeviceAsync(void* host[], void* device[], size_t size, size_t number)
{
    auto ret = CudaSMCopyAsync(host, device, size, number, stream_);
    if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    return Status::OK();
}

Status CudaSmStream::HostToDeviceAsync(void* host, void* device[], size_t size, size_t number)
{
    auto ret = CudaSMCopyAsync(host, device, size, number, stream_);
    if (ret != cudaSuccess) [[unlikely]] { return Status{ret, cudaGetErrorString(ret)}; }
    return Status::OK();
}

} // namespace UC::Trans
