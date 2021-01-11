/*
*   A simplified thread-safe queue data structure.
*/

#ifndef __CONCURRENT_QUEUE_HPP__
#define __CONCURRENT_QUEUE_HPP__

#include <queue>
#include <mutex>

namespace common {

  template <typename T>
  class ConcurrentQueue {

  private:
    mutable std::mutex _mutex;
    std::queue<T> _queue;

  public:
    ConcurrentQueue() {}
    ~ConcurrentQueue() {}

    ConcurrentQueue(const ConcurrentQueue &other) {
      if (this != &other)
      {
        std::lock_guard<std::mutex> guard(other._mutex);
        _queue = other._queue;
      }
    }

    ConcurrentQueue &operator=(const ConcurrentQueue &other) {
      if (this != &other)
      {
        std::lock_guard<std::mutex> guard(other._mutex);
        _queue = other._queue;
      }
      return *this;
    }

    void Push(const T &t) {
      std::lock_guard<std::mutex> guard(_mutex);
      _queue.push(t);
    }

    void Pop() {
      std::lock_guard<std::mutex> guard(_mutex);
      if (!_queue.empty())
      {
        _queue.pop();
      }
    }

    bool Empty() const {
      std::lock_guard<std::mutex> guard(_mutex);
      return _queue.empty();
    }

    bool Front(T &t) const {
      std::lock_guard<std::mutex> guard(_mutex);
      if (_queue.empty())
      {
        return false;
      }
      t = _queue.front();
      return true;
    }
  };
}
#endif