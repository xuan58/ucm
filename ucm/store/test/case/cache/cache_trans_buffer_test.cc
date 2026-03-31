class UCCacheTransBufferTest : public testing::TestWithParam<bool> {
public:
    UC::Test::Detail::Random rd;
};

INSTANTIATE_TEST_CASE_P(SharedCondition, UCCacheTransBufferTest, ::testing::Values(false, true));

TEST_P(UCCacheTransBufferTest, GetFirstNode)
{
    UC::CacheStore::TransBuffer transBuffer;
    UC::CacheStore::Config config;
    config.uniqueId = rd.RandomString(10);
    config.shardSize = 32768;
    config.bufferCapacity = config.shardSize * 32768;
    config.shareBufferEnable = GetParam();
    config.deviceId = 0;
    auto s = transBuffer.Setup(config);
    ASSERT_EQ(s, UC::Status::OK());
    auto blockId = UC::Test::Detail::TypesHelper::MakeBlockId("a1b2c3d4e5f6789012345678901234ab");
    constexpr size_t shardIdx = 0;
    auto handle1 = transBuffer.Get(blockId, shardIdx);
    ASSERT_TRUE(handle1);
    ASSERT_TRUE(handle1.Owner());
    ASSERT_FALSE(handle1.Ready());
    auto handle2 = transBuffer.Get(blockId, shardIdx);
    ASSERT_TRUE(handle2);
    ASSERT_FALSE(handle2.Owner());
    ASSERT_FALSE(handle2.Ready());
    ASSERT_EQ(handle1.Data(), handle2.Data());
    handle1.MarkReady();
    ASSERT_TRUE(handle2.Ready());
}

TEST_P(UCCacheTransBufferTest, InsertDifferentDataRepeatedly)
{
    constexpr size_t nBatch = 2;
    constexpr size_t nBlock = 16;
    constexpr size_t nShard = 64;
    UC::CacheStore::TransBuffer transBuffer;
    UC::CacheStore::Config config;
    config.uniqueId = rd.RandomString(10);
    config.shardSize = 4096;
    config.bufferCapacity = nBlock * nShard * config.shardSize;
    config.shareBufferEnable = GetParam();
    config.deviceId = 0;
    auto s = transBuffer.Setup(config);
    ASSERT_EQ(s, UC::Status::OK());
    for (size_t iBatch = 0; iBatch < nBatch; iBatch++) {
        std::vector<UC::Detail::BlockId> blocks(nBlock);
        std::for_each(blocks.begin(), blocks.end(), [&](auto& block) {
            block = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
        });
        for (size_t iShard = 0; iShard < nShard; iShard++) {
            std::for_each(blocks.begin(), blocks.end(), [&](auto block) {
                ASSERT_FALSE(transBuffer.Exist(block, iShard));
                auto handle = transBuffer.Get(block, iShard);
                ASSERT_TRUE(handle.Owner());
                ASSERT_FALSE(handle.Ready());
                handle.MarkReady();
            });
        }
        for (size_t iShard = 0; iShard < nShard; iShard++) {
            std::for_each(blocks.begin(), blocks.end(), [&](auto block) {
                ASSERT_TRUE(transBuffer.Exist(block, iShard));
                auto handle = transBuffer.Get(block, iShard);
                ASSERT_TRUE(handle.Owner());
                ASSERT_TRUE(handle.Ready());
            });
        }
    }
}
