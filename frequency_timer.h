#pragma once

#include <thread>
#include <chrono>
#include <iostream>

template <int Hz>
using hz_to_duration = typename std::chrono::duration<int64_t, std::ratio<1, Hz> >;

template <int Hz>
class frequency_timer_t {
public:
    frequency_timer_t(std::chrono::steady_clock::time_point initial_wakeup_time = std::chrono::steady_clock::now() + hz_to_duration<Hz>(1)) :
        wakeup_time_{initial_wakeup_time} {}

    void wait(){
        auto current_time = std::chrono::steady_clock::now();
        if (current_time < wakeup_time_) {
            std::this_thread::sleep_until(wakeup_time_);
            wakeup_time_ += hz_to_duration<Hz>(1);
        }
        else {
            std::cerr << "warning: frequency_timer_t::wait() - calling thread passed wakeup time" << std::endl << std::flush;
            wakeup_time_ = std::chrono::steady_clock::now() + hz_to_duration<Hz>(1);
        }
    }

private:
    // static auto now() { return std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point wakeup_time_;
};
