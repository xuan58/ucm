namespace UC::Trans {

class Device {
public:
    Status Setup(int32_t deviceId);
    std::unique_ptr<Stream> MakeStream();
    std::shared_ptr<Stream> MakeSharedStream();
    std::unique_ptr<Stream> MakeSMStream();
    std::unique_ptr<Buffer> MakeBuffer();
};

} // namespace UC::Trans

#endif
