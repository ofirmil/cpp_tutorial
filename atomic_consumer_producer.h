#pragma once
#include <atomic>
#include <vector>

namespace robot {
/**
 * @brief atomic consumer producer to be used only by one thread or
 * two threads (one producer, one consumer)
 * Note: does not support othter synchronization flows between many threads
 * or mixed producer consumer actions
 *
 * @tparam T
 */
template <typename T>
class atomic_consumer_producer_t {
 public:
  atomic_consumer_producer_t(size_t size)
      : products_(size, T{}), produce_index_(0u), consume_index_(0u) {
    if (products_.size() < 2u) {
      throw std::runtime_error{
          "atomic_consumer_producer_t: Error container size must be > 1"};
    }
  }

  bool insert(const T& t) {
    uint32_t next_index = get_next_index(produce_index_);

    if (next_index == consume_index_.load()) {
      // buffer is full, need to wait for consumer
      return false;
    } else {
      products_[produce_index_] = t;
      produce_index_.store(next_index);
      return true;
    }
  }

  template <typename Container>
  size_t pop_all_ready_products(Container& container) {
    size_t consumed_count = 0;
    auto produce_index = produce_index_.load();
    if (consume_index_ == produce_index) {
      // buffer is empty, need to wait for producer
      return false;
    } else {
      while (consume_index_ != produce_index) {
        container.insert(container.cend(), std::move(products_[consume_index_]));
        advance(consume_index_);
      }
    }

    return consumed_count;
  }

 private:
  uint32_t get_next_index(uint32_t index) const {
    return (index + 1u) % products_.size();
  }

  void advance(std::atomic<uint32_t>& index) {
    return index.store(get_next_index(index.load()));
  }

  std::vector<T> products_;
  std::atomic<uint32_t> produce_index_;  // points to the next available index
  std::atomic<uint32_t> consume_index_;  // points to the next available index
};

}  // namespace robot