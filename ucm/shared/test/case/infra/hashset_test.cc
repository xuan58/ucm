class UCHashSetTest : public testing::Test {};

TEST_F(UCHashSetTest, Basic)
{
    UC::HashSet<int> hs;
    EXPECT_FALSE(hs.Contains(1));
    hs.Insert(1);
    EXPECT_TRUE(hs.Contains(1));
    hs.Insert(1);
    EXPECT_TRUE(hs.Contains(1));
    hs.Remove(1);
    EXPECT_FALSE(hs.Contains(1));
    hs.Remove(1);
    EXPECT_FALSE(hs.Contains(1));
}

TEST_F(UCHashSetTest, ManyKeys)
{
    constexpr int N = 100'000;
    UC::HashSet<int> hs;
    for (int i = 0; i < N; ++i) { hs.Insert(i); }
    for (int i = 0; i < N; ++i) { EXPECT_TRUE(hs.Contains(i)); }
    for (int i = 0; i < N; i += 2) { hs.Remove(i); }
    for (int i = 0; i < N; ++i) { EXPECT_EQ(hs.Contains(i), i % 2 != 0); }
}

TEST_F(UCHashSetTest, ConcurrentInsert)
{
    constexpr int ThreadN = 8;
    constexpr int PerThread = 50'000;
    UC::HashSet<int> hs;
    auto worker = [&](int offset) {
        for (int i = 0; i < PerThread; ++i) { hs.Insert(offset + i); }
    };
    std::vector<std::thread> threads;
    for (int t = 0; t < ThreadN; ++t) { threads.emplace_back(worker, t * PerThread); }
    for (auto& t : threads) t.join();
    for (int i = 0; i < ThreadN * PerThread; ++i) { EXPECT_TRUE(hs.Contains(i)); }
}

TEST_F(UCHashSetTest, StringKey)
{
    UC::HashSet<std::string> hs;
    hs.Insert("hello");
    EXPECT_TRUE(hs.Contains("hello"));
    EXPECT_FALSE(hs.Contains("world"));
    hs.Remove("hello");
    EXPECT_FALSE(hs.Contains("hello"));
}

TEST_F(UCHashSetTest, RemoveKeepsProbeChainIntact)
{
    struct BadHash {
        size_t operator()(int) const noexcept { return 0; }
    };

    UC::HashSet<int, BadHash, 0> hs;
    hs.Insert(1);
    hs.Insert(2);
    hs.Insert(3);

    hs.Remove(2);

    EXPECT_TRUE(hs.Contains(1));
    EXPECT_FALSE(hs.Contains(2));
    EXPECT_TRUE(hs.Contains(3));
}
