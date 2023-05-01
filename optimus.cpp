#include "optimus.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <thread>

#include "frequency_timer.h"
#include "atomic_consumer_producer.h"
#include "robot_config.h"
#include "motors_positions_generator.h"
#include "robot_position_generator.h"

using namespace std;

namespace robot {


  brain_t::brain_t(const string &yaml_config) :
    config_{yaml_config},
    motors_positions_products_{ motors_positions_hz * 2 / main_loop_hz }, // double the expected size for every round 
    robot_positions_products_{ robot_positions_hz * 2 / main_loop_hz } // double the expected size for every round
    {}

  void brain_t::run() {
    running_ = true;
    motors_positions_worker_ = thread{ [this]() {
      motors_positions_generator_t generator{
          motors_positions_t(config_.motors_count, 1.0) };
      frequency_timer_t<motors_positions_hz> timer;

      while (running_) {
        auto update_request = motors_positions_update_request_.exchange(nullptr);
        if (update_request) {
          generator = move(*update_request);
        }
        if (motors_positions_products_.insert(*generator)) {
          generator++;
        }
        else {
          std::cerr << "motors_positions_worker_: ERROR consumer_producer container is full" << endl << flush;
        }
        timer.wait();
      }
    } };

    robot_positions_worker_ = thread{ [this]() {
      robot_position_generator_t generator;
      frequency_timer_t<robot_positions_hz> timer;

      while (running_) {
        robot_positions_products_.insert(*generator);
        generator++;
        timer.wait();
      }
    } };

    main_loop_ = thread{ [this]() {
      frequency_timer_t<main_loop_hz> timer;
      vector<motors_positions_t> motors_positions;
      vector<robot_position_t> robot_positions;

      while (running_) {
        motors_positions_products_.pop_all_ready_products(motors_positions);
        robot_positions_products_.pop_all_ready_products(robot_positions);

        if (motors_positions.size() > 0 && robot_positions.size() > 0) {
          // There are new positions from the other two threads.
          // Need to pass them to the active net function
          {
            unique_lock l(net_functions_mutex_);
            auto match = net_functions_.find(active_net_function_);
            if (match != net_functions_.end()) {
              net_function_t fn = (*match).second;
              l.unlock();
              // pass current logged values to active net function and receive new calculated motors positions
              auto update_request = make_shared<motors_positions_t>( fn(move(motors_positions), move(robot_positions)) );

              if (update_request->size() == config_.motors_count) {
                // At this point expeced empty previous update request since motors_positions_worker_
                // should have consumed previous update request (based on assumption that main_loop_ will wait more than 100 ms until it's
                // frequency timer will expire)
                // option 2: is to wait for response from motors_positions_t but requires deadlock fix when
                // stop() request from user is sent (optional solutuion - first kill main_loop_ and then other threads,
                // but requires to have a different running_ flag for each thread)
                // option 3: is to exchange the previous request even if it was not consumed by motors_positions_worker_
                // There is some logic to option 3 since the user has received all previous locations and may set update request according to them
                shared_ptr<motors_positions_t> expected;
                bool update_request_status = motors_positions_update_request_.compare_exchange_strong(expected, update_request);
                if (!update_request_status && running_) {
                  cerr << "main_loop_t: Error - failed to set update_request while engine is still running!" << endl << flush;
                }
              }
              else {
                std::cerr << "net function returned new motors_positions_t with invalid size" << endl << flush;
              }
              // Currently allowing the external net_function to choose weather to clear the values of the databases
              // motors_positions.clear();
              // robot_positions.clear();
            }
            else {
                std::cerr << "Info: net function not found" << endl << flush;
            }
          }
        }
        else {
          cerr << "main_loop Error: atleast one of the producer threads did not provide new products" << endl << flush;
        }

        timer.wait();
      }
    } };
  }

  void brain_t::stop() {
    running_ = false;
    for (auto th : {&main_loop_, &motors_positions_worker_, &robot_positions_worker_}) {
      if(th->joinable()) {
        th->join();
      }
    }
  }
 
  bool brain_t::set_net_function(const string &name,
                                 const net_function_t &net_function) {
    bool res;

    if (config_.functions.find(name) == config_.functions.cend()) {
      std::cerr << "net function name missing from config file" << endl << flush;
      res = false;
    }
    else {
      std::scoped_lock lock{net_functions_mutex_};
      if (net_function == nullptr) {
        net_functions_.erase(name);
      }
      else {
        net_functions_[name] = net_function;    
      }

      res = true;
    }

    return res;
  }

  bool brain_t::set_active_net_function(const string &name) {
    bool res;

    if (config_.functions.find(name) != config_.functions.cend()) {
      std::scoped_lock lock{net_functions_mutex_};
      active_net_function_ = name;
      res = true;
    }
    else {
      res = false;
    }

    return res;
  }
}  // namespace robot