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

#pragma once

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <cerrno>
#include <utility>
#include <string>
#include <rclcpp/rclcpp.hpp>

namespace mg400_interface
{

class TcpSocketException : public std::logic_error
{
public:
  TcpSocketException(
    const std::string & what
  )
  : std::logic_error(what)
  {}
};

class TcpSocketHandler
{
private:
  int fd_;
  uint16_t port_;
  std::string ip_;
  std::atomic<bool> is_connected_;

public:
  TcpSocketHandler(std::string, uint16_t);

  ~TcpSocketHandler();

  void close();
  void connect(const std::chrono::nanoseconds &);
  void disConnect();
  bool isConnected() const;
  void send(const void *, uint32_t);
  bool recv(void *, uint32_t, const std::chrono::nanoseconds &);
  std::string toString();
};
}  // namespace mg400_interface
