#include <gtest/gtest.h>
#include "../include/shutdown-data.h"

// Fixture for ShutdownData tests
class ShutdownDataTest : public ::testing::Test 
{
    protected:
        ShutdownData shutdownData;
};

TEST_F(ShutdownDataTest, AddHeader) 
{
    // Add headers and check if they are correctly added
    shutdownData.addHeader("Test-Header: TestValue");
    // No direct way to test private s_headers, so this is more of an indirect test
    EXPECT_NO_THROW(shutdownData.addHeader("Another-Header: AnotherValue"));
}

TEST_F(ShutdownDataTest, SetPostData) 
{
    // Test valid choices
    EXPECT_NO_THROW(shutdownData.setPostData("accountNumber", "12345"));

    EXPECT_NO_THROW(shutdownData.setPostData("address", "SomeAddress"));

    // Test invalid choice
    EXPECT_THROW(shutdownData.setPostData("invalidChoice", "12345"), std::invalid_argument);
}

TEST_F(ShutdownDataTest, FormatElectricityData) 
{
    std::string validJson = R"({"current":{"queue":1,"subqueue":1},"graphs":{"today":{"hoursList":[{"hour":"01","electricity":1}]}}})";
    EXPECT_NO_THROW(shutdownData.formatElectricityData(validJson));
    EXPECT_EQ(shutdownData.getQueue(), 1);
    EXPECT_EQ(shutdownData.getSubqueue(), 1);
    EXPECT_EQ(shutdownData.getWontBeElectricityToday().size(), 1);

    std::string invalidJson = R"({"invalid": "data"})";
    EXPECT_THROW(shutdownData.formatElectricityData(invalidJson), std::runtime_error);
}

TEST_F(ShutdownDataTest, ProcessHour) 
{
    nlohmann::json hourData = 
    {
        {"hour", "01"},
        {"electricity", 1}
    };
    EXPECT_NO_THROW(shutdownData.processHour(hourData, true));
    EXPECT_EQ(shutdownData.getWontBeElectricityToday().size(), 1);
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
