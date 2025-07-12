#include "Threads.hpp"

namespace th {

ThreadFactory g_ThreadFactory;

void ThreadFactory::remove_thread(thread_id t) {
  std::lock_guard guard(thread_mutex);

  threads.erase(t);
  logger.log("Removed id %1%, %2% threads remaining", t, threads.size());
  if (threads.empty()) {
    canExit.unlock();
  }
}

void join() { g_ThreadFactory.join(); }

} // namespace th