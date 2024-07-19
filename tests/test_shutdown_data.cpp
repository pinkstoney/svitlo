#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "shutdown-data.h"
#include <nlohmann/json.hpp>

class MockCurl 
{
public:
    MOCK_METHOD(CURLcode, easy_perform, (CURL* curl));
    MOCK_METHOD(const char*, easy_strerror, (CURLcode code));
};

MockCurl* g_mockCurl = nullptr;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) 
{
    size_t totalSize = size * nmemb;
    auto s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

extern "C" 
{
    CURLcode curl_easy_perform(CURL* curl) 
    {
        return g_mockCurl->easy_perform(curl);
    }

    const char* curl_easy_strerror(CURLcode code) 
    {
        return g_mockCurl->easy_strerror(code);
    }
}

class ShutdownDataTest : public ::testing::Test  
{
protected:
    MockCurl mockCurl;

    void SetUp() override 
    {
        g_mockCurl = &mockCurl;
    }

    void TearDown() override 
    {
        g_mockCurl = nullptr;
    }
};

TEST_F(ShutdownDataTest, Constructor) 
{
    EXPECT_FALSE(ShutdownData::isCurlInitialized());
    
    {
        ShutdownData shutdownData;
        EXPECT_TRUE(ShutdownData::isCurlInitialized());
    }
    EXPECT_FALSE(ShutdownData::isCurlInitialized());
}

TEST_F(ShutdownDataTest, AddHeader) 
{
    ShutdownData shutdownData;
    shutdownData.addHeader("Test-Header: Value");
}

TEST_F(ShutdownDataTest, SetPostDataAccountNumber) 
{
    ShutdownData shutdownData;
    shutdownData.setPostData("accountNumber", "25012345");
}

TEST_F(ShutdownDataTest, SetPostDataAddress) 
{
    ShutdownData shutdownData;
    shutdownData.setPostData("address", "Калуш,Привокзальна,15");
}

TEST_F(ShutdownDataTest, SetPostDataInvalidChoice) 
{
    ShutdownData shutdownData;
    EXPECT_THROW(shutdownData.setPostData("invalid", "data"), std::invalid_argument);
}

TEST_F(ShutdownDataTest, SendSuccess) 
{
    ShutdownData shutdownData;
    EXPECT_CALL(mockCurl, easy_perform(testing::_))
        .WillOnce(testing::Invoke([&](CURL* curl) 
                    {
            std::string& readBuffer = shutdownData.getReadBuffer();
            WriteCallback((void*)"Mock response data", 1, 18, &readBuffer);
            return CURLE_OK;
        }));

    std::string result = shutdownData.send();
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result, "Mock response data");
}

TEST_F(ShutdownDataTest, SendFailure) 
{
    ShutdownData shutdownData;
    EXPECT_CALL(mockCurl, easy_perform(testing::_))
        .WillOnce(testing::Return(CURLE_COULDNT_CONNECT));
    EXPECT_CALL(mockCurl, easy_strerror(CURLE_COULDNT_CONNECT))
        .WillOnce(testing::Return("Couldn't connect"));

    EXPECT_THROW(shutdownData.send(), std::runtime_error);
}

TEST_F(ShutdownDataTest, ProcessRawElectricityData) 
{
    ShutdownData shutdownData;
    testing::internal::CaptureStdout();
    shutdownData.processRawElectricityData("Test Data");
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "raw data: Test Data\n");
}

TEST_F(ShutdownDataTest, ProcessHour) 
{
    ShutdownData shutdownData;
    nlohmann::json hourData = {{"electricity", 1}, {"hour", "1"}};
    shutdownData.processHour(hourData, true);
    EXPECT_EQ(shutdownData.getWontBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getWontBeElectricityToday()[0], std::make_pair(24, 1));

    hourData = {{"electricity", 2}, {"hour", "2"}};
    shutdownData.processHour(hourData, false);
    EXPECT_EQ(shutdownData.getMightBeElectricityTomorrow().size(), 1);
    EXPECT_EQ(shutdownData.getMightBeElectricityTomorrow()[0], std::make_pair(1, 2));

    hourData = {{"electricity", 0}, {"hour", "3"}};
    shutdownData.processHour(hourData, true);
    EXPECT_EQ(shutdownData.getWillBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getWillBeElectricityToday()[0], std::make_pair(2, 3));
}

TEST_F(ShutdownDataTest, FormatElectricityDataEmptyInput) 
{
    ShutdownData shutdownData;
    EXPECT_THROW(shutdownData.formatElectricityData(""), std::runtime_error);
}

TEST_F(ShutdownDataTest, FormatElectricityDataInvalidJson) 
{
    ShutdownData shutdownData;
    EXPECT_THROW(shutdownData.formatElectricityData("{invalid json}"), std::runtime_error);
}

TEST_F(ShutdownDataTest, FormatElectricityDataValidInput) 
{
    ShutdownData shutdownData;
    nlohmann::json testData = 
    {
        {"current", {
            {"queue", 1},
            {"subqueue", 2}
        }},
        {"graphs", {
            {"today", {
                {"hoursList", {
                    {{"electricity", 1}, {"hour", "1"}},
                    {{"electricity", 2}, {"hour", "2"}},
                    {{"electricity", 0}, {"hour", "3"}}
                }}
            }},
            {"tomorrow", {
                {"hoursList", {
                    {{"electricity", 1}, {"hour", "1"}},
                    {{"electricity", 2}, {"hour", "2"}},
                    {{"electricity", 0}, {"hour", "3"}}
                }}
            }}
        }}
    };

    shutdownData.formatElectricityData(testData.dump());

    EXPECT_EQ(shutdownData.getQueue(), 1);
    EXPECT_EQ(shutdownData.getSubqueue(), 2);

    EXPECT_EQ(shutdownData.getWontBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getMightBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getWillBeElectricityToday().size(), 1);

    EXPECT_EQ(shutdownData.getWontBeElectricityTomorrow().size(), 1);
    EXPECT_EQ(shutdownData.getMightBeElectricityTomorrow().size(), 1);
    EXPECT_EQ(shutdownData.getWillBeElectricityTomorrow().size(), 1);
}

TEST_F(ShutdownDataTest, AddElectricityDataToday) 
{
    ShutdownData shutdownData;
    shutdownData.addWillBeElectricityToday(5);
    shutdownData.addMightBeElectricityToday(6);
    shutdownData.addWontBeElectricityToday(7);

    EXPECT_EQ(shutdownData.getWillBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getMightBeElectricityToday().size(), 1);
    EXPECT_EQ(shutdownData.getWontBeElectricityToday().size(), 1);

    EXPECT_EQ(shutdownData.getWillBeElectricityToday()[0], std::make_pair(5, 6));
    EXPECT_EQ(shutdownData.getMightBeElectricityToday()[0], std::make_pair(6, 7));
    EXPECT_EQ(shutdownData.getWontBeElectricityToday()[0], std::make_pair(7, 8));
}

TEST_F(ShutdownDataTest, AddElectricityDataTomorrow) 
{
    ShutdownData shutdownData;
    shutdownData.addWillBeElectricityTomorrow(5);
    shutdownData.addMightBeElectricityTomorrow(6);
    shutdownData.addWontBeElectricityTomorrow(7);

    EXPECT_EQ(shutdownData.getWillBeElectricityTomorrow().size(), 1);
    EXPECT_EQ(shutdownData.getMightBeElectricityTomorrow().size(), 1);
    EXPECT_EQ(shutdownData.getWontBeElectricityTomorrow().size(), 1);

    EXPECT_EQ(shutdownData.getWillBeElectricityTomorrow()[0], std::make_pair(5, 6));
    EXPECT_EQ(shutdownData.getMightBeElectricityTomorrow()[0], std::make_pair(6, 7));
    EXPECT_EQ(shutdownData.getWontBeElectricityTomorrow()[0], std::make_pair(7, 8));
}

TEST_F(ShutdownDataTest, SetQueueAndSubqueue) 
{
    ShutdownData shutdownData;
    shutdownData.setQueue(3);
    shutdownData.setSubqueue(4);

    EXPECT_EQ(shutdownData.getQueue(), 3);
    EXPECT_EQ(shutdownData.getSubqueue(), 4);

}

