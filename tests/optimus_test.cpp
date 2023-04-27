#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <random>

#include "optimus.h"

using namespace robot;
using namespace std;

template <typename ...Args>
string format(const char* fmt, Args ...args) {
  int size = snprintf(NULL, 0, fmt, args...);
  vector<char> buffer(size + 1, '\0');
  auto res = sprintf(buffer.data(), fmt, args...);
  if (res != size) {
    throw std::runtime_error("format function failed");
  }
  
  // TODO check how to avoid copy of the data
  return string(buffer.data());
}

template <typename T>
string print_vector(const vector<T>& v, char delimiter = ' ') {
  ostringstream os;
  auto it = v.cbegin();
  if (it != v.cend()) {
    os << *it;
    it++;
  }

  for (; it != v.cend(); it++) {
    os << delimiter << *it;
  }

  return os.str();
}

TEST(OptimusTestHelper, print_vector) {
  vector<double> v({1.1, 2.2, 3.3, 4.0004});
  auto str = print_vector(v);
  cerr << str << endl << flush;
}

TEST(OptimusTest, temp) {
  using namespace robot;
  brain_t brain{"tests/config_test.yaml"};
  brain_t::net_function_t log = [](vector<motors_positions_t>&& a, vector<robot_position_t>&& b)->motors_positions_t {
    if (a.size() > 0)
      cerr << "motors_positions size: " << a.size() << " a.back(): " << print_vector(a.back()) << endl << flush;
    else
      cerr << "motors_positions size: " << a.size() << endl << flush;

    if (b.size() > 0)
      cerr << "robot_positions size: " << b.size() << " [ " << print_vector(b) << " ]" << endl << flush;
    else
      cerr << "robot_positions size: " << b.size() << endl << flush;

    // cerr << format("motors_positions size: %s", a.size()) << endl << flush; 
    // cerr << format("robot_positions size: %s", b.size()) << endl << flush; 
    // cerr << "motors_positions: "; 
    // ostringstream os;
    // for (size_t i = 0; i < 10; i++) {
    //   if (a.size() > i) {
    //     string s = print_vector(a[i]);
    //     os << "{" << s << "} ";
    //   }
    // }
    // cerr << endl << flush;

    // cerr << "robot_positions: ";
    // cerr << print_vector(vector<robot_position_t>(b.cbegin(), b.cbegin() + 10));
    // cerr << endl << flush;
    auto res = a.size() ? a[0] : motors_positions_t{ (motors_positions_t::size_type)5u, 5.0 };
    a.clear();
    b.clear();
    return res;
  };

  ASSERT_TRUE(brain.set_active_net_function("LOG"));
  ASSERT_TRUE(brain.set_net_function("LOG", log));

  brain.run();
  this_thread::sleep_for(chrono::seconds(2));
  brain.stop();
  cerr << "Done test" << endl << flush;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
