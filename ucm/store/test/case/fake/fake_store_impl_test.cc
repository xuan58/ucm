class UCFakeStoreImplTest : public testing::Test {
protected:
    UC::Test::Detail::Random rd;
};

TEST_F(UCFakeStoreImplTest, Lookup)
{
    UC::FakeStore::FakeStore store;
    UC::Detail::Dictionary config;
    config.Set("unique_id", rd.RandomString(10));
    config.SetNumber("buffer_number", size_t(1024));
    ASSERT_TRUE(store.Setup(config).Success());
    std::vector<UC::Detail::BlockId> blocks(3);
    std::for_each(blocks.begin(), blocks.end(), [](auto& block) {
        block = UC::Test::Detail::TypesHelper::MakeBlockIdRandomly();
    });
    {
        auto foundIdx = store.LookupOnPrefix(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(foundIdx, -1);
        auto founds = store.Lookup(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(founds.size(), blocks.size());
        std::for_each(founds.begin(), founds.end(), [](auto found) { ASSERT_FALSE(found); });
    }
    UC::Detail::TaskDesc desc;
    desc.brief = "test";
    std::for_each(blocks.begin(), blocks.end(), [&desc](const auto& block) {
        desc.push_back(UC::Detail::Shard{block, 0, {nullptr}});
    });
    auto handle = store.Dump(desc).Value();
    ASSERT_TRUE(store.Wait(handle).Success());
    {
        auto foundIdx = store.LookupOnPrefix(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(foundIdx, 2);
        auto founds = store.Lookup(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(founds.size(), blocks.size());
        std::for_each(founds.begin(), founds.end(), [](auto found) { ASSERT_TRUE(found); });
    }
    auto pos = blocks.begin();
    std::advance(pos, 2);
    blocks.insert(pos, UC::Test::Detail::TypesHelper::MakeBlockIdRandomly());
    {
        auto foundIdx = store.LookupOnPrefix(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(foundIdx, 1);
        auto founds = store.Lookup(blocks.data(), blocks.size()).Value();
        ASSERT_EQ(founds.size(), blocks.size());
        std::vector<uint8_t> expected{true, true, false, true};
        ASSERT_TRUE(founds == expected);
    }
}
