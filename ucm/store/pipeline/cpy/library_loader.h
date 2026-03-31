namespace UC::PipelineStore {

template <class Interface>
class LibraryLoader {
public:
    LibraryLoader(std::string path, std::string func)
        : path_{std::move(path)}, func_{std::move(func)}
    {
    }
    ~LibraryLoader()
    {
        if (handle_) {
            dlclose(handle_);
            handle_ = nullptr;
        }
    }
    LibraryLoader(LibraryLoader&& other) noexcept
    {
        path_ = std::move(other.path_);
        func_ = std::move(other.func_);
        handle_ = other.handle_;
        other.handle_ = nullptr;
        maker_ = other.maker_;
        other.maker_ = nullptr;
    }
    LibraryLoader& operator=(LibraryLoader&& other) noexcept
    {
        if (this != &other) {
            if (handle_) { dlclose(handle_); }
            path_ = std::move(other.path_);
            func_ = std::move(other.func_);
            handle_ = other.handle_;
            other.handle_ = nullptr;
            maker_ = other.maker_;
            other.maker_ = nullptr;
        }
        return *this;
    }
    Status LoadLibrary()
    {
        handle_ = dlopen(path_.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle_) {
            return Status::Error(fmt::format("failed to load `{}`: {}", path_, dlerror()));
        }
        void* symbol = dlsym(handle_, func_.c_str());
        if (!symbol) {
            return Status::Error(fmt::format("cannot find `{}`: {}", func_, dlerror()));
        }
        maker_ = reinterpret_cast<MakerFn>(symbol);
        return Status::OK();
    }
    std::shared_ptr<Interface> CreateObject() { return std::shared_ptr<Interface>(maker_()); }

private:
    using MakerFn = Interface* (*)();
    std::string path_;
    std::string func_;
    void* handle_{nullptr};
    MakerFn maker_{nullptr};
};

}  // namespace UC::PipelineStore

#endif
