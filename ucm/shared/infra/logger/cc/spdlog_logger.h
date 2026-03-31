namespace UC::Logger {

constexpr size_t HASH_SLOT_NUM = 512;
constexpr size_t HASH_CHAIN_LEN = 4;
enum class Level { DEBUG, INFO, WARN, ERROR, CRITICAL };
struct SourceLocation {
    const char* file = "";
    const char* func = "";
    const int32_t line = 0;
};

class Logger {
    std::shared_ptr<spdlog::logger> logger_;
    std::mutex mutex_;

public:
    Logger()
    {
        logger_ = nullptr;
        register_at_exit();
    }

    void register_at_exit()
    {
        std::signal(SIGSEGV, &_signal_handler);
        std::signal(SIGABRT, &_signal_handler);
        std::signal(SIGFPE, &_signal_handler);
        std::signal(SIGILL, &_signal_handler);
        std::signal(SIGINT, &_signal_handler);
    }
    static void _signal_handler(int signum) { Logger::GetInstance().Flush(); }

    void Log(Level&& lv, SourceLocation&& loc, std::string&& msg);
    void Setup(const std::string& path, int max_files, int max_size);
    void Flush();

    static Logger& GetInstance()
    {
        static Logger inst;
        return inst;
    }

    bool IsEnabledFor(Level lv);

    bool FilterCallSite(const char* file, int line);

private:
    struct ChainEntryData {
        std::atomic<uint64_t> key_hash{0};
        std::atomic<uint64_t> rate_limit_state{0};
    };

    struct SlotData {
        std::array<ChainEntryData, HASH_CHAIN_LEN> chain_entries;
    };

    std::array<SlotData, HASH_SLOT_NUM> hash_slots_;

    std::shared_ptr<spdlog::logger> Make();
    std::string path_{"log"};
    int max_files_{3};
    int max_size_{5 * 1048576};  // 5MB
};

}  // namespace UC::Logger

#endif