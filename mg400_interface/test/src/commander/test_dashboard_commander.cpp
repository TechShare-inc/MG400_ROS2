// Copyright 2022 HarvestX Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gmock/gmock.h>
#include <mg400_interface/commander/dashboard_commander.hpp>
#include <mg400_msgs/msg/robot_mode.hpp>

using ::testing::_;
using ::testing::StrEq;
using ::testing::Return;

using mg400_msgs::msg::CollisionLevel;
using mg400_msgs::msg::DOIndex;
using mg400_msgs::msg::DOStatus;
using mg400_msgs::msg::Tool;
using mg400_msgs::msg::ToolDOIndex;
using mg400_msgs::msg::User;

class MockTcpInterface : public mg400_interface::DashboardTcpInterfaceBase
{
public:
  MockTcpInterface()
  : mg400_interface::DashboardTcpInterfaceBase() {}

  MOCK_METHOD(void, sendCommand, (const std::string &), (override));
  MOCK_METHOD(std::string, recvResponse, (), (override));
};

class TestDashboardCommander : public ::testing::Test
{
protected:
  std::unique_ptr<mg400_interface::DashboardCommander> commander;
  MockTcpInterface mock;
  virtual void SetUp()
  {
    using namespace std::chrono_literals;
    this->commander =
      std::make_unique<mg400_interface::DashboardCommander>(
      &this->mock, 1ms);
  }

  virtual void TearDown() {}
};

TEST_F(TestDashboardCommander, EnableRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("EnableRobot()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},EnableRobot();"));

  ASSERT_NO_THROW(
    commander->enableRobot());
}

TEST_F(TestDashboardCommander, DisableRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("DisableRobot()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},DisableRobot();"));

  ASSERT_NO_THROW(
    commander->disableRobot());
}

TEST_F(TestDashboardCommander, ClearError) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ClearError()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},ClearError();"));

  ASSERT_NO_THROW(commander->clearError());
}

TEST_F(TestDashboardCommander, ResetRobot) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ResetRobot()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},ResetRobot();"));

  ASSERT_NO_THROW(
    commander->resetRobot());
}

TEST_F(TestDashboardCommander, SpeedFactor) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SpeedFactor(78)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},SpeedFactor(78);"));

  ASSERT_NO_THROW(
    commander->speedFactor(78));
}

TEST_F(TestDashboardCommander, User) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("User(1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},User(1);"));

  ASSERT_NO_THROW(
    commander->user(User::USER1));
}

TEST_F(TestDashboardCommander, Tool) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("Tool(1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},Tool(1);"));

  commander->tool(Tool::TOOL1);
}

TEST_F(TestDashboardCommander, RobotMode) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("RobotMode()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{5},RobotMode();"));

  ASSERT_EQ(
    commander->robotMode(),
    mg400_msgs::msg::RobotMode::ENABLE);
}

TEST_F(TestDashboardCommander, PayLoad) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("PayLoad(3.000,0.400)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},PayLoad(3.000,0.400);"));

  ASSERT_NO_THROW(commander->payload(3, 0.4));
}

TEST_F(TestDashboardCommander, DO) {
  EXPECT_CALL(
    mock, sendCommand(StrEq("DO(12,1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},DO(12,1);"));

  ASSERT_NO_THROW(
    commander->DO(
      DOIndex::D12, DOStatus::HIGH));
}

TEST_F(TestDashboardCommander, ToolDOExecute) {
  EXPECT_CALL(
    mock, sendCommand(StrEq("ToolDOExecute(2,1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},ToolDOExecute(2,1);"));
  ASSERT_NO_THROW(
    commander->toolDOExecute(
      ToolDOIndex::D2, DOStatus::HIGH));
}

TEST_F(TestDashboardCommander, AccJ) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("AccJ(50)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},AccJ(50);"));
  ASSERT_NO_THROW(commander->accJ(50));
}

TEST_F(TestDashboardCommander, AccL) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("AccL(50)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},AccL(50);"));
  ASSERT_NO_THROW(commander->accL(50));
}

TEST_F(TestDashboardCommander, SpeedJ) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SpeedJ(50)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},SpeedJ(50);"));
  ASSERT_NO_THROW(commander->speedJ(50));
}

TEST_F(TestDashboardCommander, SpeedL) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SpeedL(50)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},SpeedL(50);"));

  ASSERT_NO_THROW(commander->speedL(50));
}
/*
TEST_F(TestDashboardCommander, Arch) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("Arch(1)"))).Times(1);
  commander->arch(mg400_interface::ArchIndex::ARCH1);
}
*/
TEST_F(TestDashboardCommander, CP) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("CP(50)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},CP(50);"));
  ASSERT_NO_THROW(commander->cp(50));
}
/*
TEST_F(TestDashboardCommander, RunScript) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("RunScript(demo)"))).Times(1);
  commander->runScript("demo");
}

TEST_F(TestDashboardCommander, StopScript) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("StopScript()"))).Times(1);
  commander->stopScript();
}

TEST_F(TestDashboardCommander, PauseScript) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("PauseScript()"))).Times(1);
  commander->pauseScript();
}

TEST_F(TestDashboardCommander, ContinueScript) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ContinueScript()"))).Times(1);
  commander->continueScript();
}
*/
TEST_F(TestDashboardCommander, SetCollisionLevel) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SetCollisionLevel(1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},SetCollisionLevel(1);"));
  ASSERT_NO_THROW(
    commander->setCollisionLevel(
      CollisionLevel::LEVEL1));
}

TEST_F(TestDashboardCommander, GetAngle) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetAngle()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return(
      "0,"
      "{0.000000,0.000000,45.000000,45.000000,0.000000,0.000000}"
      ",GetAngle();"));
  const auto ret = commander->getAngle();
  ASSERT_DOUBLE_EQ(ret.at(0), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(1), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(2), 0.25 * M_PI);
  ASSERT_DOUBLE_EQ(ret.at(3), 0.25 * M_PI);
}

TEST_F(TestDashboardCommander, GetPose) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetPose()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return(
      "0,"
      "{350.000000,0.000000,0.000000,0.000000,0.000000,0.000000}"
      ",GetPose();"));
  const auto ret = commander->getPose();
  ASSERT_DOUBLE_EQ(ret.at(0), 0.35);
  ASSERT_DOUBLE_EQ(ret.at(1), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(2), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(3), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(4), 0.0);
  ASSERT_DOUBLE_EQ(ret.at(5), 0.0);
}

TEST_F(TestDashboardCommander, EmergencyStop) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("EmergencyStop()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{},EmergencyStop();"));
  ASSERT_NO_THROW(
    commander->emergencyStop());
}
/*
TEST_F(TestDashboardCommander, ModbusCreate) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ModbusCreate(127.0.0.1,60000,1,true)"))).Times(1);
  commander->modbusCreate("127.0.0.1", 60000, 1, 1);
}

TEST_F(TestDashboardCommander, ModbusClose) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("ModbusClose(0)"))).Times(1);
  commander->modbusClose("0");
}

TEST_F(TestDashboardCommander, GetInBits) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetInBits(0,3000,5)"))).Times(1);
  commander->getInBits(0, 3000, 5);
}

TEST_F(TestDashboardCommander, GetInRegs) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetInRegs(0,4000,5)"))).Times(1);
  commander->getInRegs(0, 4000, 5, "U16");
}

TEST_F(TestDashboardCommander, GetCoils) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetCoils(0,1000,5)"))).Times(1);
  commander->getCoils(0, 1000, 5);
}

TEST_F(TestDashboardCommander, SetCoils) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SetCoils(0,1000,3,{1,0,1})"))).Times(1);
  commander->setCoils(0, 1000, 3, "1,0,1");
}

TEST_F(TestDashboardCommander, GetHoldRegs) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetHoldRegs(0,3095,1)"))).Times(1);
  commander->getHoldRegs(0, 3095, 1, "U16");
}

TEST_F(TestDashboardCommander, SetHoldRegs) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("SetHoldRegs(0,3095,2,{6000,300},U16)"))).Times(1);
  commander->setHoldRegs(0, 3095, 2, "6000,300", "U16");
}
*/
TEST_F(TestDashboardCommander, DI) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("DI(1)"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{0},DI(1);"));
  const int ret = commander->DI(1);
  ASSERT_EQ(ret, 0);
}

TEST_F(TestDashboardCommander, GetErrorID) {
  EXPECT_CALL(
    mock, sendCommand(
      StrEq("GetErrorID()"))).Times(1);
  EXPECT_CALL(
    mock, recvResponse()).WillOnce(
    Return("0,{[[69,72],[1],[],[],[],[]]},GetErrorID();"));
  const auto ret = commander->getErrorId();
  ASSERT_EQ(ret.at(0).at(0), 69);
  ASSERT_EQ(ret.at(0).at(1), 72);
  ASSERT_EQ(ret.at(1).at(0), 1);
  ASSERT_TRUE(ret.at(2).empty());
  ASSERT_TRUE(ret.at(3).empty());
  ASSERT_TRUE(ret.at(4).empty());
  ASSERT_TRUE(ret.at(5).empty());
}
