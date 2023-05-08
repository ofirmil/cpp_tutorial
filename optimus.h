#pragma once

// #include <pybind11/pybind11.h>
#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "atomic_consumer_producer.h"
#include "robot_config.h"
#include "robot_types.h"

using namespace std;

namespace robot {

class brain_t final {
 public:
  /**
   * @brief
   *
   * @param motors_positions - all motors positions from previous call to a net
   * function
   * @param robot_positions - all robot positions from previous call to a net
   * function
   * @return motors_positions_t required new motors positions
   */
  // typedef motors_positions_t
  // (*net_function_sig)(vector<motors_positions_t>&&,
  // vector<robot_position_t>&&);
  //  using net_function_t = typename std::function< net_function_sig >;
  using net_function_t = typename std::function<motors_positions_t(
      vector<motors_positions_t>&&, vector<robot_position_t>&&)>;

  brain_t(const string& yaml_config);
  brain_t() = delete;
  brain_t(const brain_t&) = delete;
  brain_t& operator=(const brain_t&) = delete;

  /**
   * @brief Set the net function object
   *
   * @param name
   * @param net_function, if the net function object is empty, then an already
   * registered net function for @name will be removed
   * @return true if @name is defined in yaml configuration
   * @return false otherwise
   */
  bool set_net_function(const string& name, const net_function_t& net_function);

  /**
   * @brief sets the active net function
   * @throw runtime_exception if 'name' is not inside yaml configuration
   *
   * @param name
   * @return true if there is a registered net function for @name
   * @return false - otherwise
   */
  bool set_active_net_function(const string& name);

  // run engine
  void run();

  // stop engine - wait for proper cleanup of running threads
  void stop();

 private:
  static constexpr uint8_t main_loop_hz = 10u;
  static constexpr uint8_t motors_positions_hz = 100u;
  static constexpr uint8_t robot_positions_hz = 250u;

  // shared objects between main loop and public API
  robot_config_t config_;
  std::string active_net_function_;
  std::map<string, net_function_t> net_functions_;
  std::mutex net_functions_mutex_;

  // shared objects between main loop and motors_positions_worker_ loop
  std::atomic<shared_ptr<motors_positions_t>> motors_positions_update_request_;
  atomic_consumer_producer_t<motors_positions_t> motors_positions_products_;

  // shared objects between main loop and motors_positions_worker_ loop
  atomic_consumer_producer_t<robot_position_t> robot_positions_products_;

  // threads fired by run() function
  std::thread motors_positions_worker_;
  std::thread robot_positions_worker_;
  std::thread main_loop_;
  atomic<bool> running_;
};

}  // namespace robot
