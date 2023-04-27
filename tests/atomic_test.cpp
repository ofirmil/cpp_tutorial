#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <atomic>
#include <memory>

// struct motors_positions_update_request_t {
//   public:
//   using request_type = typename pair< promise<>, motors_positions_t >;
//   using request_type_ptr = typename shared_ptr<request_type>;
//   std::future<> set(motors_potsitions_t&& new_val) {
//     request_type_ptr expected = nullptr;
//     request_type_ptr desired = make_shared<request_type>(make_pair(promise<>(), move(new_val)));
//     future<> f = desired->first.get_future();

//     if (!request_.compare_exchange_strong(expected, desired)) {        
//       p.set_error(std::runtime_error("update_request: Failed, already pending update request"));
//     }
//     return f;
//   }

//   pair<bool, motors_positions_t> get() {
//     pair<bool, motors_positions_t> res;
//     auto update_request = motors_positions_update_request_.exchange(nullptr);
//     if (update_request != nullptr) {
//       update_request->first.set_value();
//       res = make_pair(true, move(update_request->second));
//     }
//     else {
//       update_request->first.set_value();
//       res = {false, motors_positions_};
//     }
//   }
//   private:
//   std::atomic<request_type_ptr> request_ = nullptr;
// };

using namespace std;
TEST(AtomicTest, wait) {
  atomic<shared_ptr<vector<double>>> update_request;
  thread t1{[&update_request]{
    while (true) {
        auto prev_val = update_request.exchange(nullptr);
        if (prev_val != nullptr) {
            cerr << "t1: got vector: " << prev_val->size() << endl << flush;
            for (const auto& v: *prev_val) {
                cerr << "t1: prev_val: " << v << endl << flush;
            }
            update_request.notify_one();
            return;
        }
        else {
            cerr << "t1: sleeping 1 sec" << endl << flush;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
  }};

  auto new_val = make_shared<vector<double>>(initializer_list<double>({1.3, 2.7}));
  update_request.wait(new_val);
  cerr << "t0: wait on {1.0, 2.0} returned" << endl << flush;
  cerr << "t0: sleeping 3 sec" << endl << flush;
  this_thread::sleep_for(chrono::seconds(3));
  update_request = new_val;
  update_request.wait(new_val);
  t1.join();
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
