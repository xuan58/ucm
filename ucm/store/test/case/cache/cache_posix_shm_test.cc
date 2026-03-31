class UCCachePosixShmTest : public testing::Test {
public:
    UC::Test::Detail::Random rd;
};

TEST_F(UCCachePosixShmTest, ShmMapAndUnmap)
{
    auto fileName = rd.RandomString(20);
    UC::CacheStore::PosixShm file1{fileName};
    UC::CacheStore::PosixShm file2{fileName};
    const size_t data = 0xfffffffe;
    const auto openFlags =
        UC::CacheStore::PosixShm::OpenFlag::READ_WRITE | UC::CacheStore::PosixShm::OpenFlag::CREATE;
    void* addr1 = nullptr;
    void* addr2 = nullptr;
    ASSERT_TRUE(file1.ShmOpen(openFlags).Success());
    ASSERT_TRUE(file1.Truncate(sizeof(data)).Success());
    ASSERT_TRUE(file1.MMap(addr1, sizeof(data), true, true, true).Success());
    ASSERT_TRUE(file2.ShmOpen(openFlags).Success());
    ASSERT_TRUE(file2.MMap(addr2, sizeof(data), false, true, true).Success());
    file1.ShmUnlink();
    file2.ShmUnlink();
    *((size_t*)addr1) = data;
    ASSERT_EQ(*(size_t*)addr2, data);
    UC::CacheStore::PosixShm::MUnmap(addr1, sizeof(data));
    UC::CacheStore::PosixShm::MUnmap(addr2, sizeof(data));
}
