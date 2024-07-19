#include <gtest/gtest.h>
#include "database-manager.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

class DatabaseManagerTest : public ::testing::Test 
{
protected:
    std::string dbPath = "test.db";
    std::unique_ptr<DatabaseManager> dbManager;

    void SetUp() override 
    {
        std::remove(dbPath.c_str());
        dbManager = std::make_unique<DatabaseManager>(dbPath);
    }

    void TearDown() override 
    {
        std::remove(dbPath.c_str());
    }
};

TEST_F(DatabaseManagerTest, DatabaseInitialization) 
{
    ASSERT_TRUE(dbManager->doesTableExist("UserInfo")) << "UserInfo table was not created.";
    ASSERT_TRUE(dbManager->doesTableExist("ElectricityInfo")) << "ElectricityInfo table was not created.";
}

TEST_F(DatabaseManagerTest, SaveAndRetrieveUserInfo) 
{
    std::string info = "Test User";
    dbManager->saveUserInfo(info);

    auto allInfo = dbManager->getAllUserInfo();
    ASSERT_EQ(allInfo.size(), 1);
    EXPECT_EQ(allInfo[0].first, info);

    std::string retrievedInfo = dbManager->getUserInfo(1);
    EXPECT_EQ(retrievedInfo, info);

    EXPECT_TRUE(dbManager->isUserInfoExist(info));
}

TEST_F(DatabaseManagerTest, DeleteUserInfo) 
{
    std::string info = "Test User";
    dbManager->saveUserInfo(info);
    dbManager->deleteUserInfo(info);

    EXPECT_FALSE(dbManager->isUserInfoExist(info));
    EXPECT_TRUE(dbManager->isDatabaseEmpty());
}

TEST_F(DatabaseManagerTest, SaveAndRetrieveElectricityInfo) 
{
    std::string info = "Test Info";
    std::string date = "2023-07-19";
    int hour = 12;
    int status = 1;
    int queue = 2;
    int subqueue = 3;
    bool isTomorrow = false;

    dbManager->saveElectricityInfo(info, date, hour, status, queue, subqueue, isTomorrow);

    auto electricityInfo = dbManager->getElectricityInfo(info, isTomorrow);
    ASSERT_EQ(electricityInfo.size(), 1);
    EXPECT_EQ(std::get<0>(electricityInfo[0]), date);
    EXPECT_EQ(std::get<1>(electricityInfo[0]), hour);
    EXPECT_EQ(std::get<2>(electricityInfo[0]), status);
    EXPECT_EQ(std::get<3>(electricityInfo[0]), queue);
    EXPECT_EQ(std::get<4>(electricityInfo[0]), subqueue);
}

TEST_F(DatabaseManagerTest, SetAndRetrieveHomeUserInfo) 
{
    std::string info = "Home User";
    dbManager->saveUserInfo(info);
    dbManager->setHomeUserInfo(info);

    std::string homeInfo = dbManager->getHomeUserInfo();
    EXPECT_EQ(homeInfo, info);

    dbManager->removeHomeUserInfo();
    homeInfo = dbManager->getHomeUserInfo();
    EXPECT_TRUE(homeInfo.empty());
}

TEST_F(DatabaseManagerTest, InvalidDatabasePath) 
{
    EXPECT_THROW(DatabaseManager("invalid/path/to/db"), std::runtime_error);
}

TEST_F(DatabaseManagerTest, EmptyInfo) 
{
    std::string emptyInfo = "";
    EXPECT_THROW(dbManager->saveUserInfo(emptyInfo), std::invalid_argument);
}

TEST_F(DatabaseManagerTest, LargeInfo) 
{
    std::string largeInfo(10000, 'a');
    dbManager->saveUserInfo(largeInfo);

    auto allInfo = dbManager->getAllUserInfo();
    ASSERT_EQ(allInfo.size(), 1);
    EXPECT_EQ(allInfo[0].first, largeInfo);
}


TEST_F(DatabaseManagerTest, UpdateElectricityInfo) 
{
    std::string info = "Test Info";
    std::string date = "2023-07-19";
    int hour = 12;
    int status = 1;
    int queue = 2;
    int subqueue = 3;
    bool isTomorrow = false;

    dbManager->saveElectricityInfo(info, date, hour, status, queue, subqueue, isTomorrow);

    std::string newDate = "2023-07-20";
    int newStatus = 0;
    int newQueue = 1;
    int newSubqueue = 0;

    dbManager->saveElectricityInfo(info, newDate, hour, newStatus, newQueue, newSubqueue, isTomorrow);

    auto electricityInfo = dbManager->getElectricityInfo(info, isTomorrow);
    ASSERT_EQ(electricityInfo.size(), 1);
    EXPECT_EQ(std::get<0>(electricityInfo[0]), newDate);
    EXPECT_EQ(std::get<1>(electricityInfo[0]), hour);
    EXPECT_EQ(std::get<2>(electricityInfo[0]), newStatus);
    EXPECT_EQ(std::get<3>(electricityInfo[0]), newQueue);
    EXPECT_EQ(std::get<4>(electricityInfo[0]), newSubqueue);
}
