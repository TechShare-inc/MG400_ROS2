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

#include "mg400_interface/realtime_commander.hpp"

namespace mg400_interface
{

RealtimeCommander::RealtimeCommander(const std::string & ip)
: current_joints_{}, rt_data_{}, is_running_(false)
{
  this->tcp_socket_ = std::make_shared<TcpSocketHandler>(ip, this->PORT_);
}

RealtimeCommander::~RealtimeCommander()
{
  this->is_running_ = false;
  this->thread_->join();
}

rclcpp::Logger RealtimeCommander::getLogger()
{
  return rclcpp::get_logger("Realtime Commander");
}

void RealtimeCommander::init() noexcept
{
  try {
    this->is_running_ = true;
    this->thread_ = std::make_unique<std::thread>(
      &RealtimeCommander::recvData, this);
  } catch (const TcpSocketException & err) {
    RCLCPP_ERROR(this->getLogger(), "%s", err.what());
  }
}

void RealtimeCommander::recvData()
{
  int failed_cnt = 0;
  while (failed_cnt < this->CONNECTION_TRIAL_) {
    try {
      if (this->tcp_socket_->isConnected()) {
        if (this->tcp_socket_->recv(
            &this->rt_data_, sizeof(this->rt_data_), 5000))
        {
          if (this->rt_data_.len != 1440) {
            continue;
          }

          this->mutex_.lock();
          for (uint64_t i = 0; i < 6; ++i) {
            this->current_joints_[i] =
              this->rt_data_.q_actual[i] * TO_RADIAN;
          }
          this->mutex_.unlock();
          failed_cnt = 0;
          continue;
        } else {
          // timeout
          RCLCPP_WARN(this->getLogger(), "Tcp recv timeout");
        }
      } else {
        try {
          this->tcp_socket_->connect();
        } catch (const TcpSocketException & err) {
          RCLCPP_ERROR(
            this->getLogger(), "Tcp recv error : %s", err.what());
          using namespace std::chrono_literals;
          rclcpp::sleep_for(1s);
        }
      }
    } catch (const TcpSocketException & err) {
      this->tcp_socket_->disConnect();
      RCLCPP_ERROR(
        this->getLogger(),
        "Tcp recv error : %s", err.what());
    }
    failed_cnt += 1;
  }

  RCLCPP_ERROR(
    this->getLogger(),
    "Failed more than %d times... Close connection.",
    this->CONNECTION_TRIAL_);
  std::terminate();
}

void RealtimeCommander::sendCommand(const std::string & cmd)
{
  this->tcp_socket_->send(cmd.data(), cmd.size());
}

void RealtimeCommander::getCurrentJointStates(std::array<double, 6> & joints)
{
  this->mutex_.lock();
  memcpy(
    joints.data(), this->current_joints_.data(),
    this->current_joints_.size());
  this->mutex_.unlock();
}

RealTimeData RealtimeCommander::getRealtimeData()
{
  return this->rt_data_;
}

// DOBOT MG400 Official Command ---------------------------------------------
void RealtimeCommander::movJ(
  const double x, const double y, const double z,
  const double rx, const double ry, const double rz)
{
  std::string buf;
  buf.resize(100);
  snprintf(
    buf.data(), buf.size(),
    "MovJ(%.3lf, %.3lf, %.3lf, %.3lf, %.3lf, %.3lf)",
    x, y, z, rx, ry, rz);
  this->sendCommand(buf);
}

void RealtimeCommander::moveJog(const std::string & axis_id)
{
  std::string buf;
  buf.resize(100);
  snprintf(buf.data(), buf.size(), "MoveJog(%s)", axis_id.c_str());
  this->sendCommand(buf);
}
// End DOBOT MG400 Official Command -----------------------------------------

}  // namespace mg400_interface
