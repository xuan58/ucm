namespace UC::CacheStore {

class BufferManager {
    std::unique_ptr<TransBuffer> buffer_{nullptr};
    StoreV1* backend_{nullptr};

    template <auto LookupFunc>
    auto LookupThrough(const Detail::BlockId* blocks, size_t num)
    {
        StopWatch sw;
        auto res = (backend_->*LookupFunc)(blocks, num);
        if (!res) [[unlikely]] { return decltype(res)(res.Error()); }
        UC_DEBUG("Cache lookup({}) in backend costs {:.3f}ms.", num, sw.Elapsed().count() * 1e3);
        return res;
    }

public:
    Status Setup(const Config& config)
    {
        backend_ = config.storeBackend;
        if (config.deviceId == -1 && !config.shareBufferEnable) { return Status::OK(); }
        try {
            buffer_ = std::make_unique<TransBuffer>();
        } catch (const std::exception& e) {
            return Status::Error(fmt::format("failed({}) to make buffer", e.what()));
        }
        return buffer_->Setup(config);
    }
    TransBuffer* GetTransBuffer() { return buffer_ ? buffer_.get() : nullptr; }
    Expected<std::vector<uint8_t>> Lookup(const Detail::BlockId* blocks, size_t num)
    {
        if (!buffer_) { return LookupThrough<&StoreV1::Lookup>(blocks, num); }
        return LookupFast(blocks, num);
    }
    Expected<ssize_t> LookupOnPrefix(const Detail::BlockId* blocks, size_t num)
    {
        if (!buffer_) { return LookupThrough<&StoreV1::LookupOnPrefix>(blocks, num); }
        return LookupOnPrefixFast(blocks, num);
    }

private:
    void Lookup(const Detail::BlockId* blocks, size_t num, std::vector<uint8_t>& results,
                std::vector<Detail::BlockId>& missBlk, std::vector<size_t>& missIdx)
    {
        results.reserve(num);
        missBlk.reserve(num);
        missIdx.reserve(num);
        StopWatch sw;
        for (size_t i = 0; i < num; ++i) {
            uint8_t hit = buffer_->Exist(blocks[i], 0);
            results.push_back(hit);
            if (hit) { continue; }
            missBlk.push_back(blocks[i]);
            missIdx.push_back(i);
        }
        UC_DEBUG("Cache lookup({}) costs {:.3f}ms.", num, sw.Elapsed().count() * 1e3);
    }
    Expected<std::vector<uint8_t>> LookupFast(const Detail::BlockId* blocks, size_t num)
    {
        std::vector<uint8_t> results;
        std::vector<Detail::BlockId> missBlk;
        std::vector<size_t> missIdx;
        Lookup(blocks, num, results, missBlk, missIdx);
        if (missBlk.empty()) { return results; }
        StopWatch sw;
        auto res = backend_->Lookup(missBlk.data(), missBlk.size());
        if (!res) [[unlikely]] { return res.Error(); }
        UC_DEBUG("Cache lookup({}/{}) in backend costs {:.3f}ms.", missBlk.size(), num,
                 sw.Elapsed().count() * 1e3);
        const auto& backendVec = res.Value();
        for (size_t i = 0; i < missIdx.size(); ++i) { results[missIdx[i]] = backendVec[i]; }
        return results;
    }
    Expected<ssize_t> LookupOnPrefixFast(const Detail::BlockId* blocks, size_t num)
    {
        std::vector<uint8_t> results;
        std::vector<Detail::BlockId> missBlk;
        std::vector<size_t> missIdx;
        Lookup(blocks, num, results, missBlk, missIdx);
        if (missBlk.empty()) { return static_cast<ssize_t>(num) - 1; }
        StopWatch sw;
        auto res = backend_->LookupOnPrefix(missBlk.data(), missBlk.size());
        if (!res) [[unlikely]] { return res.Error(); }
        UC_DEBUG("Cache lookup({}/{}) in backend costs {:.3f}ms.", missBlk.size(), num,
                 sw.Elapsed().count() * 1e3);
        const auto& result = res.Value();
        if (static_cast<size_t>(result + 1) == missIdx.size()) {
            return static_cast<ssize_t>(num) - 1;
        }
        return static_cast<ssize_t>(missIdx[result + 1]) - 1;
    }
};

}  // namespace UC::CacheStore

#endif
