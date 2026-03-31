namespace UC::Trans {

void SimuStream::AsyncWorker()
{
    for (;;) {
        std::unique_lock<std::mutex> lock{this->mutex_};
        this->condition_.wait(lock, [this] { return this->stop_ || !this->tasks_.empty(); });
        if (this->stop_) { return; }
        if (this->tasks_.empty()) { continue; }
        auto task = std::move(this->tasks_.front());
        this->tasks_.pop_front();
        lock.unlock();
        task();
    }
}

void SimuStream::EnqueueTask(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock{this->mutex_};
    this->tasks_.emplace_back(std::move(task));
    this->condition_.notify_one();
}

SimuStream::~SimuStream()
{
    {
        std::lock_guard<std::mutex> lock{this->mutex_};
        this->stop_ = true;
        this->condition_.notify_all();
    }
    if (this->thread_.joinable()) { this->thread_.join(); }
}

Status SimuStream::Setup()
{
    this->thread_ = std::thread{&SimuStream::AsyncWorker, this};
    return Status::OK();
}

Status SimuStream::DeviceToHost(void* device, void* host, size_t size)
{
    std::memcpy(host, device, size);
    return Status::OK();
}

Status SimuStream::DeviceToHost(void* device[], void* host[], size_t size, size_t number)
{
    for (size_t i = 0; i < number; i++) {
        auto s = this->DeviceToHost(device[i], host[i], size);
        if (s.Failure()) { return s; }
    }
    return Status::OK();
}

Status SimuStream::DeviceToHost(void* device[], void* host, size_t size, size_t number)
{
    for (size_t i = 0; i < number; i++) {
        auto pDevice = device[i];
        auto pHost = (void*)(((int8_t*)host) + size * i);
        auto s = this->DeviceToHost(pDevice, pHost, size);
        if (s.Failure()) { return s; }
    }
    return Status::OK();
}

Status SimuStream::DeviceToHostAsync(void* device, void* host, size_t size)
{
    this->EnqueueTask([=] { this->DeviceToHost(device, host, size); });
    return Status::OK();
}

Status SimuStream::DeviceToHostAsync(void* device[], void* host[], size_t size, size_t number)
{
    this->EnqueueTask([=] { this->DeviceToHost(device, host, size, number); });
    return Status::OK();
}

Status SimuStream::DeviceToHostAsync(void* device[], void* host, size_t size, size_t number)
{
    this->EnqueueTask([=] { this->DeviceToHost(device, host, size, number); });
    return Status::OK();
}

Status SimuStream::HostToDevice(void* host, void* device, size_t size)
{
    std::memcpy(device, host, size);
    return Status::OK();
}

Status SimuStream::HostToDevice(void* host[], void* device[], size_t size, size_t number)
{
    for (size_t i = 0; i < number; i++) {
        auto s = this->HostToDevice(host[i], device[i], size);
        if (s.Failure()) { return s; }
    }
    return Status::OK();
}

Status SimuStream::HostToDevice(void* host, void* device[], size_t size, size_t number)
{
    for (size_t i = 0; i < number; i++) {
        auto pHost = (void*)(((int8_t*)host) + size * i);
        auto pDevice = device[i];
        auto s = this->HostToDevice(pHost, pDevice, size);
        if (s.Failure()) { return s; }
    }
    return Status::OK();
}

Status SimuStream::HostToDeviceAsync(void* host, void* device, size_t size)
{
    this->EnqueueTask([=] { this->HostToDevice(host, device, size); });
    return Status::OK();
}

Status SimuStream::HostToDeviceAsync(void* host[], void* device[], size_t size, size_t number)
{
    this->EnqueueTask([=] { this->HostToDevice(host, device, size, number); });
    return Status::OK();
}

Status SimuStream::HostToDeviceAsync(void* host, void* device[], size_t size, size_t number)
{
    this->EnqueueTask([=] { this->HostToDevice(host, device, size, number); });
    return Status::OK();
}

Status SimuStream::AppendCallback(std::function<void(bool)> cb)
{
    this->EnqueueTask([=] { cb(true); });
    return Status::OK();
}

Status SimuStream::WaitEvent(void* event)
{
    (void)event;
    return Status::OK();
}

Status SimuStream::Synchronized()
{
    std::mutex mutex;
    std::condition_variable cv;
    bool finish = false;
    this->EnqueueTask([&] {
        std::lock_guard<std::mutex> lock{mutex};
        finish = true;
        cv.notify_one();
    });
    std::unique_lock<std::mutex> lock{mutex};
    cv.wait(lock, [&] { return finish; });
    return Status::OK();
}

} // namespace UC::Trans
