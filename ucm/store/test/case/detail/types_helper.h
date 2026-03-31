namespace UC::Test::Detail {

class TypesHelper {
public:
    static UC::Detail::BlockId MakeBlockId(const char* hex)
    {
        UC::Detail::BlockId id{};
        for (size_t i = 0; i < id.size() && hex[i]; ++i) { id[i] = static_cast<std::byte>(hex[i]); }
        return id;
    }
    static UC::Detail::BlockId MakeBlockIdRandomly()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<std::uint8_t> dist(0, 255);
        UC::Detail::BlockId id;
        for (std::size_t i = 0; i < id.size(); ++i) { id[i] = static_cast<std::byte>(dist(gen)); }
        return id;
    }
    template <typename T, std::size_t N, typename... Args>
    static auto MakeArray(Args&&... args)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>) {
            return std::array<T, N>{((void)I, T{std::forward<Args>(args)...})...};
        }(std::make_index_sequence<N>{});
    }
};

}  // namespace UC::Test::Detail

#endif
