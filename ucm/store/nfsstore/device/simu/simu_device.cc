namespace UC {
class SimuDevice : public IBufferedDevice {
    using Task = std::function<void(void)>;

public:
    SimuDevice(const int32_t deviceId, const size_t bufferSize, const size_t bufferNumber)
        : IBufferedDevice{deviceId, bufferSize, bufferNumber}
    {
    }
    Status Setup() override
    {
        auto status = IBufferedDevice::Setup();
        if (status.Failure()) { return status; }
        if (!this->backend_.SetWorkerFn([](auto& task, const auto&) { task(); }).Run()) {
            return Status::Error();
        }
        return Status::OK();
    }
    Status H2DSync(std::byte* dst, const std::byte* src, const size_t count) override
    {
        std::copy(src, src + count, dst);
        return Status::OK();
    }
    Status D2HSync(std::byte* dst, const std::byte* src, const size_t count) override
    {
        std::copy(src, src + count, dst);
        return Status::OK();
    }
    Status H2DAsync(std::byte* dst, const std::byte* src, const size_t count) override
    {
        if (dst == nullptr || src == nullptr || count == 0) {
            UC_ERROR("Invalid params: count={}.", count);
            return Status::InvalidParam();
        }
        this->backend_.Push([=] { std::copy(src, src + count, dst); });
        return Status::OK();
    }
    Status D2HAsync(std::byte* dst, const std::byte* src, const size_t count) override
    {
        if (dst == nullptr || src == nullptr || count == 0) {
            UC_ERROR("Invalid params: count={}.", count);
            return Status::InvalidParam();
        }
        this->backend_.Push([=] { std::copy(src, src + count, dst); });
        return Status::OK();
    }
    Status AppendCallback(std::function<void(bool)> cb) override
    {
        this->backend_.Push([=] { cb(true); });
        return Status::OK();
    }
    Status Synchronized() override
    {
        Latch waiter;
        waiter.Up();
        this->backend_.Push([&] { waiter.Done(nullptr); });
        waiter.Wait();
        return Status::OK();
    }
    Status H2DBatchSync(std::byte* dArr[], const std::byte* hArr[], const size_t number,
                        const size_t count) override
    {
        for (size_t i = 0; i < number; i++) {
            auto status = this->H2DSync(dArr[i], hArr[i], count);
            if (status.Failure()) { return status; }
        }
        return Status::OK();
    }
    Status D2HBatchSync(std::byte* hArr[], const std::byte* dArr[], const size_t number,
                        const size_t count) override
    {
        for (size_t i = 0; i < number; i++) {
            auto status = this->D2HSync(hArr[i], dArr[i], count);
            if (status.Failure()) { return status; }
        }
        return Status::OK();
    }

protected:
    std::shared_ptr<std::byte> MakeBuffer(const size_t size) override
    {
        return std::shared_ptr<std::byte>((std::byte*)malloc(size), free);
    }

private:
    ThreadPool<Task> backend_;
};

std::unique_ptr<IDevice> DeviceFactory::Make(const int32_t deviceId, const size_t bufferSize,
                                             const size_t bufferNumber)
{
    try {
        return std::make_unique<SimuDevice>(deviceId, bufferSize, bufferNumber);
    } catch (const std::exception& e) {
        UC_ERROR("Failed({}) to make simu device({},{},{}).", e.what(), deviceId, bufferSize,
                 bufferNumber);
        return nullptr;
    }
}

} // namespace UC
