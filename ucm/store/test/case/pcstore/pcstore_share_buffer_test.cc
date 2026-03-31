class UCPCStoreShareBufferTest : public testing::Test {
protected:
    UC::Test::Detail::Random rd;
    std::shared_ptr<UC::ShareBuffer::Reader> MakeBufferWithRandomBlockId(
        UC::ShareBuffer& shareBuffer)
    {
        auto blockId = rd.RandomString(16);
        auto path = "/tmp/block/" + blockId;
        return shareBuffer.MakeReader(blockId, path);
    }
};

TEST_F(UCPCStoreShareBufferTest, ShareBufferUsedOut)
{
    auto uniqueId = "uc-pcstore-test-" + rd.RandomString(10);
    constexpr size_t blockSize = 4096;
    constexpr size_t blockNumber = 4;
    constexpr size_t localBlockNumber = 1;
    UC::ShareBuffer shareBuffer;
    ASSERT_TRUE(shareBuffer.Setup(blockSize, blockNumber, false, uniqueId).Success());
    std::vector<std::shared_ptr<UC::ShareBuffer::Reader>> readers;
    for (size_t i = 0; i < blockNumber + localBlockNumber; i++) {
        auto reader = MakeBufferWithRandomBlockId(shareBuffer);
        ASSERT_NE(reader, nullptr);
        if (i < blockNumber) {
            ASSERT_TRUE(reader->Shared());
        } else {
            ASSERT_FALSE(reader->Shared());
        }
        readers.push_back(std::move(reader));
    }
    std::for_each(readers.begin(), readers.end(),
                  [](auto& reader) { ASSERT_NE(reader->GetData(), 0u); });
}

TEST_F(UCPCStoreShareBufferTest, ShareBufferReuse)
{
    auto uniqueId = "uc-pcstore-test-" + rd.RandomString(10);
    constexpr size_t blockSize = 4096;
    constexpr size_t blockNumber = 4;
    constexpr size_t reuseNumber = 2;
    UC::ShareBuffer shareBuffer;
    ASSERT_TRUE(shareBuffer.Setup(blockSize, blockNumber, false, uniqueId).Success());
    std::vector<std::shared_ptr<UC::ShareBuffer::Reader>> readers;
    for (size_t i = 0; i < blockNumber; ++i) {
        auto reader = MakeBufferWithRandomBlockId(shareBuffer);
        ASSERT_NE(reader, nullptr);
        ASSERT_TRUE(reader->Shared());
        readers.push_back(std::move(reader));
    }
    for (size_t i = 0; i < reuseNumber; ++i) {
        readers.pop_back();
        auto reader = MakeBufferWithRandomBlockId(shareBuffer);
        ASSERT_NE(reader, nullptr);
        ASSERT_TRUE(reader->Shared());
        readers.push_back(std::move(reader));
    }
}

TEST_F(UCPCStoreShareBufferTest, InsertShareBufferToReadTaskList)
{
    struct ReadTask {
        std::string blockId;
        std::shared_ptr<UC::ShareBuffer::Reader> reader;
    };
    auto uniqueId = "uc-pcstore-test-" + rd.RandomString(10);
    constexpr size_t blockNumber = 4;
    constexpr size_t localBlockNumber = 2;
    std::list<ReadTask> totalReadTasks;
    std::list<ReadTask> readTasks;
    UC::ShareBuffer shareBuffer;
    ASSERT_TRUE(shareBuffer.Setup(4096, blockNumber, false, uniqueId).Success());
    for (size_t i = 0; i < blockNumber + localBlockNumber; i++) {
        ReadTask task;
        task.blockId = rd.RandomString(16);
        task.reader = shareBuffer.MakeReader(task.blockId, "/tmp/block/" + task.blockId);
        ASSERT_NE(task.reader, nullptr);
        readTasks.push_back(std::move(task));
    }
    totalReadTasks.splice(totalReadTasks.end(), readTasks);
    ASSERT_EQ(totalReadTasks.size(), blockNumber + localBlockNumber);
    std::for_each(totalReadTasks.begin(), totalReadTasks.end(),
                  [](auto& task) { ASSERT_NE(task.reader->GetData(), 0u); });
    totalReadTasks.clear();
}
