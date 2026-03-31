namespace UC::Test::Detail {

class MockStore : public UC::StoreV1 {
public:
    MOCK_METHOD((UC::Status), Setup, (const UC::Detail::Dictionary&), (override));
    MOCK_METHOD((std::string), Readme, (), (const, override));
    MOCK_METHOD((UC::Expected<std::vector<uint8_t>>), Lookup,
                (const UC::Detail::BlockId* blocks, size_t num), (override));
    MOCK_METHOD((UC::Expected<ssize_t>), LookupOnPrefix,
                (const UC::Detail::BlockId* blocks, size_t num), (override));
    MOCK_METHOD(void, Prefetch, (const UC::Detail::BlockId* blocks, size_t num), (override));
    MOCK_METHOD((UC::Expected<UC::Detail::TaskHandle>), Load, (UC::Detail::TaskDesc task),
                (override));
    MOCK_METHOD((UC::Expected<UC::Detail::TaskHandle>), Dump, (UC::Detail::TaskDesc task),
                (override));
    MOCK_METHOD((UC::Expected<bool>), Check, (UC::Detail::TaskHandle taskId), (override));
    MOCK_METHOD((UC::Status), Wait, (UC::Detail::TaskHandle taskId), (override));
};

}  // namespace UC::Test::Detail

#endif
