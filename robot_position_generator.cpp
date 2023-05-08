#include "robot_position_generator.h"

#include <random>

#include "robot_types.h"

namespace robot {

robot_position_generator_t::robot_position_generator_t()
    : rand_engine_{}, dist_{0, 10}, position_(1.0) {}

robot_position_generator_t &robot_position_generator_t::operator++(int) {
  position_ = position_ * (1 + 0.001 * dist_(rand_engine_));
  return *this;
}

double robot_position_generator_t::operator*() const { return position_; }

}  // namespace robot
