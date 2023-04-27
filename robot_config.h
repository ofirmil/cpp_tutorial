#pragma once

#include <string>
#include <set>

using namespace std; 
namespace robot {
  struct robot_config_t {
    robot_config_t();
    robot_config_t(const std::string& yaml_path);
    size_t motors_count;
    set<string> functions; 
  };

  robot_config_t make_robot_config(const std::string& yaml_path);
}