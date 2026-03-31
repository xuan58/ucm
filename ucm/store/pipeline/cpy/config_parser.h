namespace UC::PipelineStore {

class ConfigParser {
public:
    static Status Parse(Detail::Dictionary& dictionary, const pybind11::dict& dict)
    {
        for (auto item : dict) {
            auto keyObj = item.first;
            auto valObj = item.second;
            std::string key = pybind11::cast<std::string>(keyObj);
            if (TryParseScalar(dictionary, key, valObj)) { continue; }
            if (pybind11::isinstance<pybind11::list>(valObj) &&
                TryParseList(dictionary, key, pybind11::cast<pybind11::list>(valObj))) {
                continue;
            }
            return Status::InvalidParam("Unsupported config value type for key({})", key);
        }
        return Status::OK();
    }

private:
    template <typename T>
    static std::vector<T> ParseList(const pybind11::list& list)
    {
        std::vector<T> vec;
        vec.reserve(list.size());
        for (auto item : list) { vec.push_back(pybind11::cast<T>(item)); }
        return vec;
    }
    static bool TryParseScalar(Detail::Dictionary& dictionary, const std::string& key,
                               const pybind11::handle& obj)
    {
        if (pybind11::isinstance<pybind11::bool_>(obj)) {
            dictionary.Set(key, pybind11::cast<bool>(obj));
            return true;
        }
        if (pybind11::isinstance<pybind11::int_>(obj)) {
            dictionary.SetNumber(key, pybind11::cast<ssize_t>(obj));
            return true;
        }
        if (pybind11::isinstance<pybind11::float_>(obj)) {
            dictionary.Set(key, pybind11::cast<double>(obj));
            return true;
        }
        if (pybind11::isinstance<pybind11::str>(obj)) {
            dictionary.Set(key, pybind11::cast<std::string>(obj));
            return true;
        }
        return false;
    }
    static bool TryParseList(Detail::Dictionary& dictionary, const std::string& key,
                             const pybind11::list& pyList)
    {
        if (pyList.empty()) {
            dictionary.Set(key, std::vector<std::any>{});
            return true;
        }
        auto first = pyList[0];
        if (pybind11::isinstance<pybind11::bool_>(first)) {
            dictionary.Set(key, ParseList<bool>(pyList));
            return true;
        }
        if (pybind11::isinstance<pybind11::int_>(first)) {
            dictionary.Set(key, ParseList<ssize_t>(pyList));
            return true;
        }
        if (pybind11::isinstance<pybind11::float_>(first)) {
            dictionary.Set(key, ParseList<double>(pyList));
            return true;
        }
        if (pybind11::isinstance<pybind11::str>(first)) {
            dictionary.Set(key, ParseList<std::string>(pyList));
            return true;
        }
        return false;
    }
};

}  // namespace UC::PipelineStore

#endif  // UNIFIEDCACHE_PIPELINE_STORE_CPY_CONFIG_PARSER_H
