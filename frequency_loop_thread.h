#pragma once

#include <thread>
#include <memory>
#include <functional>
#include "frequency_timer.h"

namespace robot {
  template <int Frequency>
  class frequency_loop_thread_t : public std::thread {
  public:
    using work_func_t = typename std::function<void(void)>;

    frequency_loop_thread_t() = default;
  
    template<typename Callable, typename... Args>
    frequency_loop_thread_t(Callable&& work, Args&& ...args) {
      change_work(std::forward<Callable>(work), std::forward<Args>(args)...);

      auto loop = [this](){
        frequency_timer_t<Frequency> timer;

        // Copy work_ to local work to avoid race when work_ is replaced from another thread
        for(auto work = work_.load(); work; work = work_.load()) {
          (*work)();
          timer.wait();
        }
     };

      static_cast<std::thread&>(*this) = std::thread{loop};
    }

    void stop_request() {
      work_.store(nullptr);
    }  

    template<typename Callable, typename... Args>
    void change_work(Callable&& work, Args&& ...args) {
      work_.store(make_shared<work_func_t>(std::bind(std::forward<Callable>(work), std::forward<Args>(args)...)));
    }  

    virtual ~frequency_loop_thread_t() = default;
  
  private:
      std::atomic<std::shared_ptr<work_func_t> > work_;
  };

} // namespace robot