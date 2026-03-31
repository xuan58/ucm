namespace UC::PosixStore {

class TransManager {
public:
    using IoEngine = Detail::TaskWrapper<TransTask, Detail::TaskHandle>;

    Status Setup(const Config& config, const SpaceLayout* layout)
    {
        if (config.ioEngine == "aio") {
            ioEngine_ = &ioEngineAio_;
            return ioEngineAio_.Setup(config, layout);
        }
        if (config.ioEngine == "psync") {
            ioEngine_ = &ioEnginePsync_;
            return ioEnginePsync_.Setup(config, layout);
        }
        return Status::InvalidParam("invalid io engine({})", config.ioEngine);
    }
    IoEngine* GetIoEngine() const { return ioEngine_; }

private:
    IoEngineAio ioEngineAio_;
    IoEnginePsync ioEnginePsync_;
    IoEngine* ioEngine_{nullptr};
};

}  // namespace UC::PosixStore

#endif
