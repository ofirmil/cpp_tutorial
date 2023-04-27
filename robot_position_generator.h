#pragma once

#include <random>
#include "robot_types.h"

namespace robot {

class robot_position_generator_t {
 public:
  robot_position_generator_t();
  robot_position_generator_t& operator++(int);
  robot_position_t operator*() const;

 private:
  std::minstd_rand rand_engine_;
  std::uniform_int_distribution<int> dist_{0, 10};
  robot_position_t position_;
};

}  // namespace robot
