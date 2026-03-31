namespace UC::Trans {

class ReservedBuffer : public Buffer {
    struct {
        Indexer indexer;
        std::shared_ptr<void> buffers;
        size_t size;
    } hostBuffers_, deviceBuffers_;

    template <typename Buffers>
    static std::shared_ptr<void> GetBufferFrom(Buffers& buffers)
    {
        auto pos = buffers.indexer.Acquire();
        if (pos != buffers.indexer.npos) {
            auto addr = static_cast<int8_t*>(buffers.buffers.get());
            auto ptr = static_cast<void*>(addr + buffers.size * pos);
            return std::shared_ptr<void>(ptr,
                                         [&buffers, pos](void*) { buffers.indexer.Release(pos); });
        }
        return nullptr;
    }

public:
    Status MakeDeviceBuffers(size_t size, size_t number) override
    {
        auto totalSize = size * number;
        auto buffers = this->MakeDeviceBuffer(totalSize);
        if (!buffers) {
            return Status::Error(fmt::format("out of memory({}) on device", totalSize));
        }
        this->deviceBuffers_.size = size;
        this->deviceBuffers_.buffers = buffers;
        this->deviceBuffers_.indexer.Setup(number);
        return Status::OK();
    }

    std::shared_ptr<void> GetDeviceBuffer(size_t size) override
    {
        if (size <= this->deviceBuffers_.size) {
            auto buffer = GetBufferFrom(this->deviceBuffers_);
            if (buffer) { return buffer; }
        }
        return this->MakeDeviceBuffer(size);
    }

    std::shared_ptr<void> MakeHostBuffer4DirectIo(size_t size) override
    {
        return this->MakeHostBuffer(size);
    }

    Status MakeHostBuffers(size_t size, size_t number) override
    {
        auto totalSize = size * number;
        auto buffers = this->MakeHostBuffer(totalSize);
        if (!buffers) { return Status::Error(fmt::format("out of memory({}) on host", totalSize)); }
        this->hostBuffers_.size = size;
        this->hostBuffers_.buffers = buffers;
        this->hostBuffers_.indexer.Setup(number);
        return Status::OK();
    }

    std::shared_ptr<void> GetHostBuffer(size_t size) override
    {
        if (size <= this->hostBuffers_.size) {
            auto buffer = GetBufferFrom(this->hostBuffers_);
            if (buffer) { return buffer; }
        }
        return this->MakeHostBuffer(size);
    }
};

}  // namespace UC::Trans

#endif
