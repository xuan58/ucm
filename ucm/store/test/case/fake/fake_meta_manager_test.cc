class UCFakeMetaManagerTest : public testing::Test {
protected:
    UC::Test::Detail::Random rd;
};

TEST_F(UCFakeMetaManagerTest, FirstInFirstEvict)
{
    constexpr auto number = size_t(4);
    UC::FakeStore::Config config;
    config.uniqueId = rd.RandomString(12);
    config.bufferNumber = number;
    config.shareBufferEnable = true;
    UC::FakeStore::MetaManager metaMgr;
    ASSERT_EQ(metaMgr.Setup(config), UC::Status::OK());
    std::vector<UC::Detail::BlockId> blocks(number + 1);
    auto end = blocks.end();
    std::for_each(blocks.begin(), end, [&](auto& block) {
        block = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
        ASSERT_FALSE(metaMgr.Exist(block));
    });
    std::for_each(blocks.begin(), end, [&](const auto& block) { metaMgr.Insert(block); });
    auto iter = blocks.begin();
    ASSERT_FALSE(metaMgr.Exist(*iter));
    iter++;
    std::for_each(iter, end, [&](const auto& block) { ASSERT_TRUE(metaMgr.Exist(block)); });
}

TEST_F(UCFakeMetaManagerTest, BlockDeduplication)
{
    constexpr auto number = size_t(2);
    UC::FakeStore::Config config;
    config.uniqueId = rd.RandomString(12);
    config.bufferNumber = number;
    config.shareBufferEnable = true;
    UC::FakeStore::MetaManager metaMgr;
    ASSERT_EQ(metaMgr.Setup(config), UC::Status::OK());
    auto block1 = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
    auto block2 = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
    ASSERT_FALSE(metaMgr.Exist(block1));
    ASSERT_FALSE(metaMgr.Exist(block2));
    metaMgr.Insert(block1);
    metaMgr.Insert(block1);
    metaMgr.Insert(block2);
    metaMgr.Insert(block2);
    ASSERT_TRUE(metaMgr.Exist(block1));
    ASSERT_TRUE(metaMgr.Exist(block2));
}
