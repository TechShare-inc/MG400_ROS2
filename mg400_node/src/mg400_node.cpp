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

#include "mg400_node/mg400_node.hpp"


namespace mg400_node
{
using namespace std::chrono_literals;   // NOLINT

MG400Node::MG400Node(const rclcpp::NodeOptions & options)
: rclcpp::Node("mg400_node", options)
{
  const std::string ip_address =
    this->declare_parameter<std::string>("ip_address", "192.168.1.6");
  RCLCPP_INFO(
    this->get_logger(), "Connecting to %s ...", ip_address.c_str());

  this->declare_parameter<std::vector<std::string>>(
    "dashboard_api_plugins", this->default_dashboard_api_plugins_);
  this->declare_parameter<std::vector<std::string>>(
    "motion_api_plugins", this->default_motion_api_plugins_);


  this->interface_ =
    std::make_shared<mg400_interface::MG400Interface>(ip_address);

  this->declare_parameter<std::string>("prefix", "");
  if (!this->interface_->configure(this->get_parameter("prefix").as_string())) {
    exit(EXIT_FAILURE);
    return;
  }

  while (!this->interface_->activate()) {
    RCLCPP_INFO(this->get_logger(), "Try reconnecting...");
    rclcpp::sleep_for(5s);
  }

  this->dashboard_api_loader_ =
    std::make_shared<mg400_plugin_base::DashboardApiLoader>();
  this->dashboard_api_loader_->loadPlugins(
    this->get_parameter("dashboard_api_plugins").as_string_array());

  this->motion_api_loader_ =
    std::make_shared<mg400_plugin_base::MotionApiLoader>();
  this->motion_api_loader_->loadPlugins(
    this->get_parameter("motion_api_plugins").as_string_array());

  this->init_timer_ = this->create_wall_timer(
    0s, std::bind(&MG400Node::onInit, this));
}

MG400Node::~MG400Node()
{
  if (this->interface_) {
    this->interface_->deactivate();
  }
}

void MG400Node::onInit()
{
  this->init_timer_->cancel();

  this->dashboard_api_loader_->configure(
    this->interface_->dashboard_commander,
    this->shared_from_this(),
    this->interface_);
  this->dashboard_api_loader_->showPluginInfo(
    this->get_node_logging_interface());

  this->motion_api_loader_->configure(
    this->interface_->motion_commander,
    this->shared_from_this(),
    this->interface_);
  this->motion_api_loader_->showPluginInfo(
    this->get_node_logging_interface());

  this->joint_state_pub_ =
    this->create_publisher<sensor_msgs::msg::JointState>(
    "joint_states", rclcpp::SystemDefaultsQoS());
  this->robot_mode_pub_ =
    this->create_publisher<mg400_msgs::msg::RobotMode>(
    "robot_mode", rclcpp::SensorDataQoS());

  this->runTimer();
}

void MG400Node::onJointStateTimer()
{
  if (this->interface_->ok()) {
    static std::array<double, 4> joint_states;
    this->interface_->realtime_tcp_interface->getCurrentJointStates(joint_states);

    this->joint_state_pub_->publish(
      mg400_interface::JointHandler::getJointState(
        joint_states,
        this->interface_->realtime_tcp_interface->frame_id_prefix));
  }
}

void MG400Node::onRobotModeTimer()
{
  if (this->interface_->ok()) {
    auto msg = std::make_unique<mg400_msgs::msg::RobotMode>();
    uint64_t mode;
    if (this->interface_->realtime_tcp_interface->getRobotMode(mode)) {
      msg->robot_mode = mode;
      this->robot_mode_pub_->publish(std::move(msg));
    }
  }
}

void MG400Node::onErrorTimer()
{
  if (this->interface_->ok()) {
    if (!this->interface_->realtime_tcp_interface->isRobotMode(
        mg400_msgs::msg::RobotMode::ERROR))
    {
      return;
    }

    try {
      std::stringstream ss;
      const auto joints_error_ids =
        this->interface_->dashboard_commander->getErrorId();
      for (size_t i = 0; i < joints_error_ids.size(); ++i) {
        if (joints_error_ids.at(i).empty()) {
          continue;
        }
        ss << "Joint" << (i + 1) << ":" << std::endl;
        for (auto error_id : joints_error_ids.at(i)) {
          const auto message =
            this->interface_->error_msg_generator->get(error_id);
          ss << "\t" << message << std::endl;
        }
      }
      RCLCPP_ERROR(this->get_logger(), ss.str().c_str());
      this->interface_->dashboard_commander->clearError();
    } catch (const std::runtime_error & ex) {
      RCLCPP_ERROR(this->get_logger(), ex.what());
    } catch (const std::out_of_range & ex) {
      RCLCPP_ERROR(this->get_logger(), "Out of range %s", ex.what());
    } catch (...) {
      RCLCPP_ERROR(this->get_logger(), "Unknown exception");
    }
  }
}

void MG400Node::onInterfaceCheckTimer()
{
  if (!this->interface_->ok()) {
    // Stop the timer and try to reconnect
    this->cancelTimer();
    this->interface_->deactivate();
    while (!this->interface_->activate()) {
      RCLCPP_INFO(this->get_logger(), "Try reconnecting...");
      rclcpp::sleep_for(5s);
    }
    this->runTimer();
  }
}

void MG400Node::runTimer()
{
  this->joint_state_timer_ = this->create_wall_timer(
    10ms, std::bind(&MG400Node::onJointStateTimer, this));
  this->robot_mode_timer_ = this->create_wall_timer(
    100ms, std::bind(&MG400Node::onRobotModeTimer, this));
  this->error_timer_ = this->create_wall_timer(
    500ms, std::bind(&MG400Node::onErrorTimer, this));
  this->interface_check_timer_ = this->create_wall_timer(
    100ms, std::bind(&MG400Node::onInterfaceCheckTimer, this));
}

void MG400Node::cancelTimer()
{
  this->joint_state_timer_.reset();
  this->robot_mode_timer_.reset();
  this->error_timer_.reset();
}

}  // namespace mg400_node
