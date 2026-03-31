namespace UC::Detail {

class Dictionary {
    std::unordered_map<std::string, std::any> data_;

    template <typename T>
    T Get(const std::string& key) const
    {
        return std::any_cast<T>(data_.find(key)->second);
    }

public:
    bool Contains(const std::string& key) const { return data_.find(key) != data_.end(); }
    template <typename T>
    void Set(const std::string& key, const T& value)
    {
        data_[key] = value;
    }
    template <typename T>
    void SetNumber(const std::string& key, const T& value)
    {
        data_[key] = static_cast<ssize_t>(value);
    }
    template <typename T>
    void Get(const std::string& key, T& target) const
    {
        if (Contains(key)) { target = Get<T>(key); }
    }
    template <typename T>
    void GetNumber(const std::string& key, T& target) const
    {
        if (Contains(key)) { target = static_cast<T>(Get<ssize_t>(key)); }
    }
    template <typename T>
    void GetNumbers(const std::string& key, std::vector<T>& target) const
    {
        if (!Contains(key)) { return; }
        const auto& v = Get<std::vector<ssize_t>>(key);
        std::for_each(v.begin(), v.end(), [&](auto d) { target.push_back(static_cast<T>(d)); });
    }
};

}  // namespace UC::Detail

#endif  // UNIFIEDCACHE_STORE_DETAIL_TYPE_DICTIONARY_H
