namespace UC::Trans {

class Buffer {
public:
    virtual ~Buffer() = default;

    virtual std::shared_ptr<void> MakeDeviceBuffer(size_t size) = 0;
    virtual Status MakeDeviceBuffers(size_t size, size_t number) = 0;
    virtual std::shared_ptr<void> GetDeviceBuffer(size_t size) = 0;

    virtual std::shared_ptr<void> MakeHostBuffer(size_t size) = 0;
    virtual std::shared_ptr<void> MakeHostBuffer4DirectIo(size_t size) = 0;
    virtual Status MakeHostBuffers(size_t size, size_t number) = 0;
    virtual std::shared_ptr<void> GetHostBuffer(size_t size) = 0;

    static Status RegisterHostBuffer(void* host, size_t size, void** pDevice = nullptr);
    static void UnregisterHostBuffer(void* host);
};

}  // namespace UC::Trans

#endif
