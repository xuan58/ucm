namespace UC::Trans {

static void* AllocMemory(size_t size, int8_t initVal)
{
    auto ptr = malloc(size);
    if (!ptr) { return nullptr; }
    std::memset(ptr, initVal, size);
    return ptr;
}

static void FreeMemory(void* ptr) { free(ptr); }

template <typename Buffers>
static std::shared_ptr<void> GetBuffer(Buffers& buffers)
{
    auto pos = buffers.indexer.Acquire();
    if (pos != buffers.indexer.npos) {
        auto addr = static_cast<int8_t*>(buffers.buffers.get());
        auto ptr = static_cast<void*>(addr + buffers.size * pos);
        return std::shared_ptr<void>(ptr, [&buffers, pos](void*) { buffers.indexer.Release(pos); });
    }
    return nullptr;
}

std::shared_ptr<void> SimuBuffer::MakeDeviceBuffer(size_t size)
{
    constexpr int8_t deviceInitVal = 0xd;
    auto device = AllocMemory(size, deviceInitVal);
    if (!device) { return nullptr; }
    return std::shared_ptr<void>(device, FreeMemory);
}

std::shared_ptr<void> SimuBuffer::MakeHostBuffer(size_t size)
{
    constexpr int8_t hostInitVal = 0xa;
    auto device = AllocMemory(size, hostInitVal);
    if (!device) { return nullptr; }
    return std::shared_ptr<void>(device, FreeMemory);
}

Status Buffer::RegisterHostBuffer(void* host, size_t size, void** pDevice)
{
    if (pDevice) { *pDevice = host; }
    return Status::OK();
}

void Buffer::UnregisterHostBuffer(void* host) {}

} // namespace UC::Trans
