#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <chrono>

#include "../frequency_timer.h"
#include "../frequency_loop_thread.h"

/* TODO: make it a test instead of an example */
TEST(FrequencyLoopThread, HzToDuration) {
  using namespace std::chrono;
  using namespace robot;
  constexpr int hz_10 = 10;
  //auto print_int = [](int i){ std::cerr << "### " << i << " ###" << std::endl << std::flush; };
  std::function print_int = [](int i){ std::cerr << "### " << i << " ###" << std::endl << std::flush; };
  frequency_loop_thread_t loop_each_100_milliseconds = 
    frequency_loop_thread_t<hz_10>(print_int, 1);
  std::this_thread::sleep_for(std::chrono::seconds(1));

  loop_each_100_milliseconds.change_work(print_int, 2);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  loop_each_100_milliseconds.stop_request();
  loop_each_100_milliseconds.join();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
