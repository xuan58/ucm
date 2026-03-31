class UCCacheBufferManagerTest : public testing::Test {
protected:
    static UC::Expected<std::vector<uint8_t>> AllMiss(const UC::Detail::BlockId* blocks, size_t num)
    {
        std::vector<uint8_t> founds(num, false);
        return founds;
    }
    static UC::Expected<std::vector<uint8_t>> AllHit(const UC::Detail::BlockId* blocks, size_t num)
    {
        std::vector<uint8_t> founds(num, true);
        return founds;
    }
};

TEST_F(UCCacheBufferManagerTest, Lookup)
{
    UC::Test::Detail::MockStore backend;
    UC::Test::Detail::Random rd;
    UC::CacheStore::BufferManager bufferMgr;
    UC::CacheStore::Config config;
    config.storeBackend = &backend;
    config.deviceId = 0;
    size_t tensorSize = 4096;
    config.tensorSizes = {4096};
    config.shardSize = tensorSize;
    config.blockSize = config.shardSize;
    config.deviceId = 0;
    config.bufferCapacity = config.shardSize * 1024;
    config.uniqueId = rd.RandomString(10);
    config.shareBufferEnable = true;
    ASSERT_TRUE(bufferMgr.Setup(config).Success());
    std::vector<UC::Detail::BlockId> blocks(3);
    std::for_each(blocks.begin(), blocks.end(), [](auto& block) {
        block = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
    });
    EXPECT_CALL(backend, LookupOnPrefix).WillOnce(testing::Return(-1));
    EXPECT_CALL(backend, Lookup).WillOnce(testing::Invoke(AllMiss));
    {
        auto foundIdx = bufferMgr.LookupOnPrefix(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(foundIdx, -1);
        auto founds = bufferMgr.Lookup(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(founds.size(), blocks.size());
        std::for_each(founds.begin(), founds.end(), [](auto found) { ASSERT_FALSE(found); });
    }
    EXPECT_CALL(backend, LookupOnPrefix).WillOnce(testing::Invoke([](auto, size_t num) {
        return static_cast<ssize_t>(num) - 1;
    }));
    EXPECT_CALL(backend, Lookup).WillOnce(testing::Invoke(AllHit));
    {
        auto foundIdx = bufferMgr.LookupOnPrefix(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(foundIdx, 2);
        auto founds = bufferMgr.Lookup(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(founds.size(), blocks.size());
        std::for_each(founds.begin(), founds.end(), [](auto found) { ASSERT_TRUE(found); });
    }
}
