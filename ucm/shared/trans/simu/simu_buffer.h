namespace UC::Trans {

class SimuBuffer : public ReservedBuffer {
public:
    std::shared_ptr<void> MakeDeviceBuffer(size_t size) override;
    std::shared_ptr<void> MakeHostBuffer(size_t size) override;
};

} // namespace UC::Trans

#endif
