namespace UC {

class NowTime {
public:
    static auto Now()
    {
        auto now = std::chrono::steady_clock::now().time_since_epoch();
        return std::chrono::duration<double>(now).count();
    }
};

}  // namespace UC

#endif
