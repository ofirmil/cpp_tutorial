#pragma once

#include "robot_types.h"

namespace robot {

struct motors_positions_generator_t {
  motors_positions_generator_t(motors_positions_t &&initial_value);
  motors_positions_generator_t& operator++(int);
  motors_positions_generator_t &operator=(motors_positions_t &&reset_val);
  const motors_positions_t &operator*() const;

 private:
  motors_positions_t motors_positions_;
};

} // namespace robot
