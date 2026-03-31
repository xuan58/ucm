namespace UC::CacheStore {

class BufferStrategy;

class TransBuffer {
    using Index = std::size_t;
    static constexpr Index npos = std::numeric_limits<Index>::max();
    std::shared_ptr<BufferStrategy> strategy_{nullptr};

public:
    class Handle {
    public:
        Handle() = default;
        Handle(const Handle& other) : buf_{other.buf_}, pos_{other.pos_}, owner_{other.owner_}
        {
            if (Valid()) { buf_->Acquire(pos_); }
        }
        Handle& operator=(const Handle& other)
        {
            if (this != &other) {
                Handle tmp(other);
                Swap(tmp);
            }
            return *this;
        }
        Handle(Handle&& other) noexcept : buf_{other.buf_}, pos_{other.pos_}, owner_{other.owner_}
        {
            other.buf_ = nullptr;
            other.pos_ = npos;
            other.owner_ = false;
        }
        Handle& operator=(Handle&& other) noexcept
        {
            Handle tmp(std::move(other));
            Swap(tmp);
            return *this;
        }
        ~Handle()
        {
            if (Valid()) { buf_->Release(pos_); }
        }
        explicit operator bool() const { return Valid(); }
        bool Owner() const { return owner_; }
        void* Data()
        {
            if (Valid()) { return buf_->DataAt(pos_); }
            return nullptr;
        }
        bool Ready() const { return buf_->Ready(pos_); };
        void MarkReady() { buf_->MarkReady(pos_); };

    private:
        friend class TransBuffer;
        Handle(TransBuffer* buf, Index pos, bool owner) : buf_{buf}, pos_{pos}, owner_{owner} {};
        bool Valid() const { return buf_ && pos_ != npos; }
        void Swap(Handle& other) noexcept
        {
            std::swap(buf_, other.buf_);
            std::swap(pos_, other.pos_);
            std::swap(owner_, other.owner_);
        }
        TransBuffer* buf_{nullptr};
        Index pos_{npos};
        bool owner_{false};
    };

public:
    Status Setup(const Config& config);
    Handle Get(const Detail::BlockId& blockId, size_t shardIdx);
    bool Exist(const Detail::BlockId& blockId, size_t shardIdx);

private:
    bool ExistAt(size_t iBucket, const Detail::BlockId& blockId, size_t shardIdx);
    size_t FindAt(size_t iBucket, const Detail::BlockId& blockId, size_t shardIdx, bool& owner);
    size_t Alloc(const Detail::BlockId& blockId, size_t shardIdx, size_t iBucket);
    void MoveTo(size_t iBucket, size_t iNode);
    void Remove(size_t iBucket, size_t iNode);
    void* DataAt(Index pos);
    void Acquire(Index pos);
    void Release(Index pos);
    bool Ready(Index pos);
    void MarkReady(Index pos);
};

}  // namespace UC::CacheStore

#endif
