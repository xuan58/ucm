namespace UC {

class CpuAffinity {
public:
    static Status SetCpuAffinity4CurrentThread(const cpu_set_t& mask)
    {
        if (CPU_COUNT(&mask) == 0) { return Status::InvalidParam(); }
        auto ret = sched_setaffinity(0, sizeof(mask), &mask);
        if (ret != 0) { return Status::Error(std::to_string(errno)); }
        std::this_thread::yield();
        return Status::OK();
    }
    static Status SetCpuAffinity4CurrentThread(const std::vector<ssize_t> cores)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        for (const auto core : cores) { CPU_SET(core, &mask); }
        return SetCpuAffinity4CurrentThread(mask);
    }
};

}  // namespace UC

#endif  // UNIFIEDCACHE_INFRA_CPU_AFFINITY_H
