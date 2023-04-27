#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <chrono>
#include <random>

#include "frequency_timer.h"
TEST(TempTest, foo) {
  auto distance = [](auto consumed, auto produced){return (10 + (produced - consumed)) % 10; };
  ASSERT_EQ(distance(1, 5), 4);
  ASSERT_EQ(distance(9, 3), 4);
}

// std::cerr << "--- milliseconds: ---" << std::endl << std::flush;
// std::cerr << " 1 sec:" << duration_cast<milliseconds>(seconds(1)).count() << std::endl << std::flush; 
// std::cerr << "100_hz:" << duration_cast<milliseconds>(hz_to_duration<100>(1)).count() << std::endl << std::flush; 
// std::cerr << "250_hz:" << duration_cast<milliseconds>(hz_to_duration<250>(1)).count() << std::endl << std::flush; 
// std::cerr << "--- micro: ---" << std::endl << std::flush;
// std::cerr << " 1 sec:" << duration_cast<microseconds>(seconds(1)).count() << std::endl << std::flush; 
// std::cerr << "100_hz:" << duration_cast<microseconds>(hz_to_duration<100>(1)).count() << std::endl << std::flush; 
// std::cerr << "250_hz:" << duration_cast<microseconds>(hz_to_duration<250>(1)).count() << std::endl << std::flush; 


TEST(FrequencyTimerTest, HzToDuration) {
  using namespace std::chrono;

  ASSERT_EQ(duration_cast<milliseconds>(hz_to_duration<10>(1)).count(), 100u);
  ASSERT_EQ(duration_cast<milliseconds>(hz_to_duration<100>(1)).count(), 10u);
  ASSERT_EQ(duration_cast<milliseconds>(hz_to_duration<250>(1)).count(), 4u);
}

TEST(FrequencyTimerTest, wait) {
  using namespace std::chrono;
  auto constexpr hz = 250u;

  auto get_rand = []()->int {
    static std::minstd_rand eng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist{0, 3999};
    return dist(eng);
  };

  frequency_timer_t<hz> f;
  auto frequency_time = system_clock::now();
  for (size_t i = 0; i < 10; i++) {
    std::this_thread::sleep_for(microseconds(get_rand()));
    f.wait();
    auto expected_freq_time = frequency_time + milliseconds(4);

    auto current_time = system_clock::now();
    ASSERT_GE(current_time, expected_freq_time);
    ASSERT_LE(current_time, expected_freq_time + microseconds(1000));

    frequency_time += hz_to_duration<hz>(1);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
