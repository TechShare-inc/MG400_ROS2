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


#include <mg400_interface/error_msg_generator.hpp>

namespace mg400_interface
{
ErrorMsgGenerator::ErrorMsgGenerator(
  const std::string & target_filename
)
: logger_(rclcpp::get_logger("ErrorMsgGenerator")),
  filename_(
    ament_index_cpp::get_package_share_directory("mg400_interface") +
    "/resources/" + target_filename)
{
  if (target_filename.empty()) {
    RCLCPP_ERROR(this->logger_, "Json file name not given");
  }

  RCLCPP_INFO(
    this->logger_, "Target file: %s",
    this->filename_.c_str());

  this->loadJsonFile();
}

std::string ErrorMsgGenerator::get(const int id)
{
  return this->error_map_.at(id);
}


bool ErrorMsgGenerator::loadJsonFile()
{
  std::fstream json_file;

  json_file.open(this->filename_, std::ios::in);

  Json::Reader reader;
  Json::Value json_data;
  if (!reader.parse(json_file, json_data, true)) {
    RCLCPP_ERROR(
      this->logger_, "Could not parse json file.");
    json_file.close();
    return false;
  }
  json_file.close();


  for (auto data : json_data) {
    RCLCPP_INFO(
      this->logger_,
      "ID %d, Message: %s",
      data["id"].asInt(),
      data["en"]["description"].asString().c_str());

    this->error_map_.emplace(
      data["id"].asInt(), data["en"]["description"].asString().c_str());
  }

  return true;
}


}  // namespace mg400_interface
