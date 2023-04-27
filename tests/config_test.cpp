#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "../robot_config.h"

using namespace std; 

std::initializer_list<string> supported_net_functions({"RESET_TO_ONE", "INCREMENT_BY_2", "LOG"});
TEST(YamlTest, RawLoad) {
  auto config = ::YAML::LoadFile("tests/config_test.yaml");
  ASSERT_EQ(config["config"]["motors-count"].as<size_t>(), 5);
  std::vector<string> v = config["config"]["functions"].as<std::vector<string>>();
  ASSERT_EQ(v.size(), 3);
  std::vector<string> expected(supported_net_functions);
  ASSERT_EQ(v, expected);
  ASSERT_EQ(v, std::vector<string>(supported_net_functions));
}

TEST(YamlTest, ExtractConfigTest) {
  auto config = robot::make_robot_config("tests/config_test.yaml");
  ASSERT_EQ(config.motors_count, 5);
  ASSERT_EQ(config.functions, std::set<string>(supported_net_functions));
}

TEST(YamlTest, ConfigConstructor) {
  robot::robot_config_t config{"tests/config_test.yaml"};
  ASSERT_EQ(config.motors_count, 5);
  ASSERT_EQ(config.functions, std::set<string>(supported_net_functions));
}

TEST(YamlTest, ConfigConstructorThrowDuplicate) {
  ASSERT_THROW(robot::robot_config_t{"tests/config_test_dup_func_name.yaml"}, std::runtime_error);
}

#include <thread>
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
