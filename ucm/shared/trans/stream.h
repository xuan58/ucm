namespace UC::Trans {

class Stream {
public:
    virtual ~Stream() = default;
    virtual Status Setup() = 0;

    virtual Status DeviceToHost(void* device, void* host, size_t size) = 0;
    virtual Status DeviceToHost(void* device[], void* host[], size_t size, size_t number) = 0;
    virtual Status DeviceToHost(void* device[], void* host, size_t size, size_t number) = 0;
    virtual Status DeviceToHostAsync(void* device, void* host, size_t size) = 0;
    virtual Status DeviceToHostAsync(void* device[], void* host[], size_t size, size_t number) = 0;
    virtual Status DeviceToHostAsync(void* device[], void* host, size_t size, size_t number) = 0;

    virtual Status HostToDevice(void* host, void* device, size_t size) = 0;
    virtual Status HostToDevice(void* host[], void* device[], size_t size, size_t number) = 0;
    virtual Status HostToDevice(void* host, void* device[], size_t size, size_t number) = 0;
    virtual Status HostToDeviceAsync(void* host, void* device, size_t size) = 0;
    virtual Status HostToDeviceAsync(void* host[], void* device[], size_t size, size_t number) = 0;
    virtual Status HostToDeviceAsync(void* host, void* device[], size_t size, size_t number) = 0;

    virtual Status AppendCallback(std::function<void(bool)> cb) = 0;
    virtual Status Synchronized() = 0;
    virtual Status WaitEvent(void* event) = 0;
};

}  // namespace UC::Trans

#endif
