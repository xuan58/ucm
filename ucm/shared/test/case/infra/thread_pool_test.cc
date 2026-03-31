class UCThreadPoolTest : public ::testing::Test {};

TEST_F(UCThreadPoolTest, TimeoutDetection)
{
    struct TestTask {
        int taskId;
        std::atomic<bool>* finished;
        std::atomic<bool>* timeout;
    };

    constexpr size_t nWorker = 2;
    constexpr size_t timeoutMs = 20;
    std::atomic<int> timeoutCount{0};
    std::atomic<bool> taskFinished{false};
    std::atomic<bool> taskTimeout{false};

    UC::ThreadPool<TestTask> threadPool;
    threadPool.SetNWorker(nWorker)
        .SetWorkerFn([](TestTask& task, const auto&) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            *(task.finished) = true;
        })
        .SetWorkerTimeoutFn(
            [&](TestTask& task, const auto) {
                timeoutCount++;
                task.timeout->store(true);
            },
            timeoutMs, 10)
        .Run();
    std::list<TestTask> tasks{
        {1, &taskFinished, &taskTimeout}
    };
    threadPool.Push(tasks);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_GT(timeoutCount.load(), 0);
    ASSERT_TRUE(taskTimeout.load());
}

TEST_F(UCThreadPoolTest, SimulatedFileSystemHang)
{
    struct TestTask {
        std::atomic<bool>* simulatingHang;
    };

    std::atomic<int> hangDetected{0};
    constexpr size_t hangTimeoutMs = 20;
    std::atomic<bool> taskHang{true};

    UC::ThreadPool<TestTask> threadPool;
    threadPool.SetNWorker(1)
        .SetWorkerFn([](TestTask& task, const auto&) {
            std::mutex fakeMutex;
            std::unique_lock<std::mutex> fakelock(fakeMutex);
            std::condition_variable fakeCond;
            while (*(task.simulatingHang)) {
                fakeCond.wait_for(fakelock, std::chrono::milliseconds(10));  // waiting forever
            }
        })
        .SetWorkerTimeoutFn(
            [&](TestTask& task, const auto) {
                hangDetected++;
                *(task.simulatingHang) = false;  // stop simulating hang
            },
            hangTimeoutMs, 10)
        .Run();
    std::list<TestTask> tasks{{&taskHang}};
    threadPool.Push(tasks);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_GT(hangDetected.load(), 0);
}