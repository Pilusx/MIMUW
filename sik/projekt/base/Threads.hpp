#ifndef BASE_THREADS_HPP_
#define BASE_THREADS_HPP_

#include "Logging.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace th {

typedef uint64_t thread_id;

class Thread;

/**
 * Manages threads.
 */
class ThreadFactory {
  std::mutex thread_mutex;
  std::mutex canExit;
  std::map<thread_id, std::shared_ptr<Thread>> threads;
  thread_id cnt;

#ifdef LOG_THREADS
  typedef info::Logger<true> Log;
#else
  typedef info::Logger<false> Log;
#endif
  Log logger;

public:
  ThreadFactory() : cnt(0), logger("Thread", std::cout) {}

  template <typename... Ts> void add_thread(Ts... ts) {
    std::lock_guard guard(thread_mutex);

    if (!threads.size()) {
      canExit.lock();
    }
    thread_id id = cnt++;
    threads[id] = std::make_shared<Thread>(id, std::move(ts)...);
    logger.log("Assigned id %1%, %2% threads remaining", id, threads.size());
  }

  void remove_thread(thread_id t);

  void join() {
    {
      std::lock_guard guard(thread_mutex);
      logger.log("Waiting on %1% threads to finish.", threads.size());
    }
    canExit.lock();
    canExit.unlock();
  }
};

extern ThreadFactory g_ThreadFactory;

class OnThreadExit {
  thread_id id;

public:
  explicit OnThreadExit(thread_id id) : id(id) {}

  ~OnThreadExit() { g_ThreadFactory.remove_thread(id); }
};

class Thread : public std::thread {
public:
  template <typename F, typename... Ts>
  Thread(thread_id id, F f, Ts... ts)
      : std::thread(
            [](thread_id id, F f, Ts... ts) {
              thread_local OnThreadExit tex(id);
              f(std::move(ts)...);
            },
            id, f, std::move(ts)...) {
    detach();
  }
};

template <typename... Ts> inline void spawn(Ts... ts) {
  g_ThreadFactory.add_thread(std::move(ts)...);
}

/**
 * Wait until all threads finish.
 */
void join();

} // namespace th

#endif // BASE_THREADS_HPP_