#include "motors_positions_generator.h"

#include <atomic>

#include "robot_config.h"

using namespace std;

namespace robot {

motors_positions_generator_t::motors_positions_generator_t(
    motors_positions_t &&initial_value)
    : motors_positions_(move(initial_value)) {}

motors_positions_generator_t &motors_positions_generator_t::operator++(int) {
  // TODO consider using Eigen vector, may be faster calculation or postpone
  // the actual calculation to the main loop (assuming the calculation is time
  // consuming)
  for (auto &motor_pos : motors_positions_) {
    motor_pos *= 1.1;
  }

  return *this;
}

motors_positions_generator_t &motors_positions_generator_t::operator=(
    motors_positions_t &&reset_val) {
  motors_positions_ = reset_val;
  return *this;
}

const motors_positions_t &motors_positions_generator_t::operator*() const {
  return motors_positions_;
}

}  // namespace robot
