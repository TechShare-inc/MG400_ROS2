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

#include "mg400_interface/tcp_interface/motion_tcp_interface.hpp"

namespace mg400_interface
{

MotionTcpInterface::MotionTcpInterface(const std::string & ip)
{
  this->is_running_.store(false);
  this->tcp_socket_ = std::make_shared<TcpSocketHandler>(ip, this->PORT_);
}

MotionTcpInterface::~MotionTcpInterface()
{
  this->disConnect();
}

rclcpp::Logger MotionTcpInterface::getLogger()
{
  return rclcpp::get_logger("Motion Tcp Interface");
}

void MotionTcpInterface::init() noexcept
{
  try {
    this->is_running_.store(true);
    this->thread_ = std::make_unique<std::thread>(&MotionTcpInterface::checkConnection, this);
  } catch (const TcpSocketException & err) {
    RCLCPP_ERROR(this->getLogger(), "%s", err.what());
  }
}

void MotionTcpInterface::checkConnection()
{
  using namespace std::chrono_literals; // NOLINT
  while (this->is_running_.load()) {
    try {
      if (!this->tcp_socket_->isConnected()) {
        this->tcp_socket_->connect(1s);
      } else {
        rclcpp::sleep_for(1s);
        continue;
      }
    } catch (const TcpSocketException & err) {
      this->tcp_socket_->disConnect();
      RCLCPP_ERROR(this->getLogger(), "Tcp recv error : %s", err.what());
      return;
    }
  }
}

bool MotionTcpInterface::isConnected()
{
  return this->tcp_socket_->isConnected();
}

void MotionTcpInterface::disConnect()
{
  this->is_running_.store(false);
  if (this->thread_->joinable()) {
    this->thread_->join();
  }
  this->tcp_socket_->disConnect();
  RCLCPP_INFO(this->getLogger(), "Close connection.");
}

void MotionTcpInterface::sendCommand(const std::string & cmd)
{
  this->tcp_socket_->send(cmd.data(), cmd.size());
}
}  // namespace mg400_interface
