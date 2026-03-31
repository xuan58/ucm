namespace UC::Test::Detail {

class Random {
public:
    std::string RandomString(const size_t length)
    {
        const std::string allowedChars =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
        std::mt19937 gen(this->rd_());
        std::uniform_int_distribution<> dis(0, allowedChars.length() - 1);
        std::string randomString(length, 0);
        for (size_t i = 0; i < length; i++) { randomString[i] = allowedChars[dis(gen)]; }
        return randomString;
    }

private:
    std::random_device rd_;
};

}  // namespace UC::Test::Detail

#endif
