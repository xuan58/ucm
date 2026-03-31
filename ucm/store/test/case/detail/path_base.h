namespace UC::Test::Detail {

class PathBase : public ::testing::Test {
public:
    void SetUp() override
    {
        testing::Test::SetUp();
        const auto info = testing::UnitTest::GetInstance()->current_test_info();
        std::string testCaseName = info->test_case_name();
        std::string testName = info->name();
        this->path_ = "./" + testCaseName + "_" + testName + "_" + this->rd_.RandomString(20) + "/";
        system((std::string("rm -rf ") + this->path_).c_str());
        system((std::string("mkdir -p ") + this->path_).c_str());
    }
    void TearDown() override
    {
        system((std::string("rm -rf ") + this->path_).c_str());
        testing::Test::TearDown();
    }
    std::string Path() const { return this->path_; }

private:
    Random rd_;
    std::string path_;
};

}  // namespace UC::Test::Detail

#endif
