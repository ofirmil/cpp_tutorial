#include "robot_config.h"

#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <string>
#include <vector>

namespace YAML {
template <>
struct convert<::robot::robot_config_t> {
  static Node encode(const ::robot::robot_config_t &config) {
    Node node;
    node["config"] = "";
    node["config"]["motors-count"] = config.motors_count;
    for (const auto &net_function_name : config.functions) {
      node["config"]["functions"].push_back(net_function_name);
    }

    return node;
  }

  static bool decode(const Node &node, ::robot::robot_config_t &config) {
    config.motors_count = node["config"]["motors-count"].as<size_t>();
    std::set<string> s;
    for (const auto &net_function_name : node["config"]["functions"]) {
      if (s.insert(std::move(net_function_name.as<string>())).second == false) {
        throw std::runtime_error("dubplicated net_functions in yaml");
      }
    }
    config.functions = move(s);
    return true;
  }
};
}  // namespace YAML

namespace robot {
robot_config_t::robot_config_t() : motors_count{0}, functions{} {}

robot_config_t::robot_config_t(const std::string &yaml_path) {
  try {
    *this = ::YAML::LoadFile(yaml_path).as<robot_config_t>();
  } catch (const std::runtime_error &parse_err) {
    std::string err{"Failed parsing input file: "};
    err += yaml_path;
    err += ", yaml error: ";
    err += parse_err.what();
    throw std::runtime_error{err};
  }
}

robot_config_t make_robot_config(const std::string &yaml_path) {
  try {
    return ::YAML::LoadFile(yaml_path).as<robot_config_t>();
  } catch (const std::runtime_error &parse_err) {
    std::string err{"Failed parsing input file: "};
    err += yaml_path;
    err += ", yaml error: ";
    err += parse_err.what();
    throw std::runtime_error{err};
  }
}

}  // namespace robot
