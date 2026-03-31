class UCPosixStoreTest : public UC::Test::Detail::PathBase {};

TEST_F(UCPosixStoreTest, SetupWithInvalidParam)
{
    using namespace UC::PosixStore;
    {
        UC::Detail::Dictionary config;
        PosixStore store;
        ASSERT_EQ(store.Setup(config), UC::Status::InvalidParam());
    }
    {
        UC::Detail::Dictionary config;
        config.Set("storage_backends", std::vector<std::string>{Path()});
        config.SetNumber("device_id", 0);
        PosixStore store;
        ASSERT_EQ(store.Setup(config), UC::Status::InvalidParam());
    }
    {
        UC::Detail::Dictionary config;
        config.Set("storage_backends", std::vector<std::string>{Path()});
        config.SetNumber("device_id", 0);
        config.SetNumber("tensor_size", size_t(4096));
        config.SetNumber("shard_size", size_t(4096));
        config.SetNumber("block_size", size_t(4096));
        config.Set("posix_io_engine", std::string("psync"));
        config.SetNumber("posix_data_trans_concurrency", size_t(0));
        PosixStore store;
        ASSERT_EQ(store.Setup(config), UC::Status::InvalidParam());
    }
}

TEST_F(UCPosixStoreTest, DumpThenLoad)
{
    using namespace UC::PosixStore;
    UC::Detail::Dictionary config;
    config.SetNumber("device_id", 0);
    config.Set("storage_backends", std::vector<std::string>{Path()});
    constexpr size_t dataSize = 32768;
    config.SetNumber("tensor_size", dataSize);
    config.SetNumber("shard_size", dataSize);
    config.SetNumber("block_size", dataSize);
    PosixStore store;
    auto s = store.Setup(config);
    ASSERT_EQ(s, UC::Status::OK());
    auto block = UC::Test::Detail::TypesHelper::MakeBlockId("a1b2c3d4e5f6789012345678901234ab");
    constexpr size_t nBlocks = 1;
    auto founds = store.Lookup(&block, nBlocks);
    ASSERT_TRUE(founds.HasValue());
    ASSERT_EQ(founds.Value(), std::vector<uint8_t>{false});
    UC::Test::Detail::DataGenerator data1{nBlocks, dataSize};
    data1.GenerateRandom();
    UC::Detail::TaskDesc desc1;
    desc1.brief = "Dump";
    desc1.push_back(UC::Detail::Shard{block, 0, {data1.Buffer()}});
    auto handle1 = store.Dump(desc1);
    ASSERT_TRUE(handle1.HasValue());
    s = store.Wait(handle1.Value());
    ASSERT_EQ(s, UC::Status::OK());
    founds = store.Lookup(&block, nBlocks);
    ASSERT_TRUE(founds.HasValue());
    ASSERT_EQ(founds.Value(), std::vector<uint8_t>{true});
    UC::Test::Detail::DataGenerator data2{nBlocks, dataSize};
    data2.Generate();
    UC::Detail::TaskDesc desc2;
    desc2.brief = "Load";
    desc2.push_back(UC::Detail::Shard{block, 0, {data2.Buffer()}});
    auto handle2 = store.Load(desc2);
    ASSERT_TRUE(handle2.HasValue());
    s = store.Wait(handle2.Value());
    ASSERT_EQ(s, UC::Status::OK());
    ASSERT_EQ(data1.Compare(data2), 0);
}

TEST_F(UCPosixStoreTest, DumpThenLoadWithIoDirect)
{
    using namespace UC::PosixStore;
    UC::Detail::Dictionary config;
    config.SetNumber("device_id", 0);
    config.Set("storage_backends", std::vector<std::string>{Path()});
    constexpr size_t dataSize = 32768;
    config.SetNumber("tensor_size", dataSize);
    config.SetNumber("shard_size", dataSize);
    config.SetNumber("block_size", dataSize);
    config.Set("io_direct", true);
    PosixStore store;
    auto s = store.Setup(config);
    ASSERT_EQ(s, UC::Status::OK());
    auto block = UC::Test::Detail::TypesHelper::MakeBlockId("a1b2c3d4e5f6789012345678901234ab");
    constexpr size_t nBlocks = 1;
    auto founds = store.Lookup(&block, nBlocks);
    ASSERT_TRUE(founds.HasValue());
    ASSERT_EQ(founds.Value(), std::vector<uint8_t>{false});
    void* buffer1 = nullptr;
    auto ret = posix_memalign(&buffer1, 4096, dataSize);
    ASSERT_EQ(ret, 0);
    *(size_t*)buffer1 = 0xfffffffe;
    UC::Detail::TaskDesc desc1;
    desc1.brief = "Dump";
    desc1.push_back(UC::Detail::Shard{block, 0, {buffer1}});
    auto handle1 = store.Dump(desc1);
    ASSERT_TRUE(handle1.HasValue());
    s = store.Wait(handle1.Value());
    ASSERT_EQ(s, UC::Status::OK());
    founds = store.Lookup(&block, nBlocks);
    ASSERT_TRUE(founds.HasValue());
    ASSERT_EQ(founds.Value(), std::vector<uint8_t>{true});
    void* buffer2 = nullptr;
    ret = posix_memalign(&buffer2, 4096, dataSize);
    ASSERT_EQ(ret, 0);
    *(size_t*)buffer2 = 0x00000001;
    UC::Detail::TaskDesc desc2;
    desc2.brief = "Load";
    desc2.push_back(UC::Detail::Shard{block, 0, {buffer2}});
    auto handle2 = store.Load(desc2);
    ASSERT_TRUE(handle2.HasValue());
    s = store.Wait(handle2.Value());
    ASSERT_EQ(s, UC::Status::OK());
    ASSERT_EQ(*(size_t*)buffer1, *(size_t*)buffer2);
    free(buffer1);
    free(buffer2);
}
