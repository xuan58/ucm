class UCSpscRingQueueTest : public testing::Test {};

TEST_F(UCSpscRingQueueTest, Basic)
{
    UC::SpscRingQueue<size_t> queue;
    queue.Setup(16);
    size_t data;
    ASSERT_FALSE(queue.TryPop(data));
    ASSERT_TRUE(queue.TryPush(1023));
    ASSERT_TRUE(queue.TryPop(data));
    ASSERT_EQ(data, 1023);
    ASSERT_FALSE(queue.TryPop(data));
}

TEST_F(UCSpscRingQueueTest, FIFO)
{
    UC::SpscRingQueue<size_t> queue;
    queue.Setup(16);
    constexpr size_t nElem = 10;
    for (size_t i = 0; i < nElem; i++) { ASSERT_TRUE(queue.TryPush(std::move(i))); }
    for (size_t i = 0; i < nElem; i++) {
        size_t value = -1;
        ASSERT_TRUE(queue.TryPop(value));
        ASSERT_EQ(value, i);
    }
    size_t value = -1;
    ASSERT_FALSE(queue.TryPop(value));
}

TEST_F(UCSpscRingQueueTest, Full)
{
    constexpr size_t N = 10;
    UC::SpscRingQueue<size_t> queue;
    queue.Setup(N);
    constexpr size_t nElem = N - 1;
    for (size_t i = 0; i < nElem; i++) { ASSERT_TRUE(queue.TryPush(std::move(i))); }
    ASSERT_FALSE(queue.TryPush(999));
    size_t value = -1;
    ASSERT_TRUE(queue.TryPop(value));
    ASSERT_EQ(value, 0);
    ASSERT_TRUE(queue.TryPush(999));
}

TEST_F(UCSpscRingQueueTest, MoveOnly)
{
    struct MoveOnly {
        int value;
        MoveOnly() = default;
        explicit MoveOnly(int v) : value(v) {}
        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
        MoveOnly(MoveOnly&&) = default;
        MoveOnly& operator=(MoveOnly&&) = default;
    };
    UC::SpscRingQueue<MoveOnly> queue;
    queue.Setup(9);
    EXPECT_TRUE(queue.TryPush(MoveOnly(42)));
    MoveOnly obj;
    EXPECT_TRUE(queue.TryPop(obj));
    EXPECT_EQ(obj.value, 42);
}
