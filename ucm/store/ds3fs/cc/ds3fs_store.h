namespace UC::Ds3fsStore {

class Ds3fsStoreImpl;
class Ds3fsStore : public StoreV1 {
public:
    ~Ds3fsStore() override;
    Status Setup(const Detail::Dictionary& config) override;
    std::string Readme() const override;
    Expected<std::vector<uint8_t>> Lookup(const Detail::BlockId* blocks, size_t num) override;
    void Prefetch(const Detail::BlockId* blocks, size_t num) override;
    Expected<Detail::TaskHandle> Load(Detail::TaskDesc task) override;
    Expected<Detail::TaskHandle> Dump(Detail::TaskDesc task) override;
    Expected<bool> Check(Detail::TaskHandle taskId) override;
    Status Wait(Detail::TaskHandle taskId) override;

private:
    std::shared_ptr<Ds3fsStoreImpl> impl_;
};

}  // namespace UC::Ds3fsStore

#endif
