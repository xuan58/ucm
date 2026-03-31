namespace UC::Test::Detail {

class DataGenerator {
public:
    explicit DataGenerator(const size_t nPage, const size_t pageSize = 4096)
        : nPage_{nPage}, pageSize_{pageSize}, data_{nullptr}
    {
    }
    ~DataGenerator()
    {
        if (this->data_) {
            free(this->data_);
            this->data_ = nullptr;
        }
    }
    void Generate()
    {
        this->data_ = malloc(this->Size());
        assert(this->data_ != nullptr);
    }
    void GenerateRandom()
    {
        this->Generate();
        for (size_t i = 0; i < this->nPage_; i++) {
            *(size_t*)((char*)this->data_ + this->pageSize_ * i) = i;
        }
    }
    int32_t Compare(const DataGenerator& other)
    {
        if (this->nPage_ < other.nPage_) { return -1; }
        if (this->nPage_ < other.nPage_) { return 1; }
        for (size_t i = 0; i < this->nPage_; i++) {
            auto ret = memcmp((char*)this->data_ + this->pageSize_ * i,
                              (char*)other.data_ + this->pageSize_ * i, this->pageSize_);
            if (ret != 0) { return ret; }
        }
        return 0;
    }
    size_t Size() const { return this->pageSize_ * this->nPage_; }
    void* Buffer() const { return this->data_; }

private:
    size_t nPage_;
    size_t pageSize_;
    void* data_;
};

}  // namespace UC::Test::Detail

#endif
