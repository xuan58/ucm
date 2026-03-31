namespace UC::PosixStore {

class AioImpl {
public:
    struct Result {
        ssize_t nBytes;
        int32_t error;
    };
    using Callback = std::function<void(Result)>;
    struct Io {
        int32_t fd;
        uint64_t offset;
        uint32_t length;
        void* buffer;
        Callback callback;
    };

    ~AioImpl();
    Status Setup();
    Status ReadAsync(Io&& io);
    Status WriteAsync(Io&& io);

private:
    void CompletionLoop();
    void HarvestCompletions(std::vector<io_event>& events);
    Status SubmitIo(struct iocb* cb);

    size_t queueDepth_{4096};
    size_t epollTimeoutMs{10};
    size_t batchCompleteSize{512};
    aio_context_t ctx_{0};
    int32_t eventFd_{-1};
    int32_t epollFd_{-1};
    std::atomic_bool stop_{false};
    std::thread eventThread_;
};

}  // namespace UC::PosixStore

#endif
