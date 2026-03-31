namespace UC::FakeStore {

struct Config {
    std::string uniqueId{};
    size_t bufferNumber{1048576};
    bool shareBufferEnable{true};
};

}  // namespace UC::FakeStore

#endif
