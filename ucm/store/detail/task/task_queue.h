namespace UC {

class TaskQueue {
public:
    virtual ~TaskQueue() = default;
    virtual void Push(std::list<Task::Shard>& shards) noexcept = 0;
};

} // namespace UC

#endif
