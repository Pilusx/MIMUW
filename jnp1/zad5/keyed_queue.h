#ifndef KEYED_QUEUE_H
#define KEYED_QUEUE_H

#include <exception>
#include <list>
#include <map>
#include <memory>
#include <vector>

class lookup_error : public std::exception {
  const char *what() const throw() override {
    return "lookup_error";
  }
};

template <class K, class V>
class keyed_queue {
private:
  class keyed_queue_impl;
  class copy_guard;

  std::shared_ptr<keyed_queue_impl> queuePtr;
  bool enforceCopy = false;

public:
  using k_iterator = typename keyed_queue_impl::iterator;

  keyed_queue() {
    keyed_queue_impl kqi;
    queuePtr = std::make_shared<keyed_queue_impl>(kqi);
  }

  keyed_queue(keyed_queue const &other) {
    if (other.enforceCopy) {
      keyed_queue_impl kqi(*other.queuePtr);
      queuePtr = std::make_shared<keyed_queue_impl>(kqi);
    } else {
      queuePtr = other.queuePtr;
    }
    enforceCopy = false;
  }

  keyed_queue(keyed_queue &&other) {
    queuePtr = std::move(other.queuePtr);
    enforceCopy = other.enforceCopy;
  }

  keyed_queue &operator=(keyed_queue other) {
    enforceCopy = other.enforceCopy;
    std::swap(queuePtr, other.queuePtr);
    return *this;
  }

  void push(K const &k, V const &v) {
    copy_guard guard(&queuePtr, &enforceCopy);
    queuePtr->push(k, v);
    guard.setSuccess();
  }

  void pop() {
    copy_guard guard(&queuePtr, &enforceCopy);
    queuePtr->pop();
    guard.setSuccess();
  }

  void pop(K const &k) {
    copy_guard guard(&queuePtr, &enforceCopy);
    queuePtr->pop(k);
    guard.setSuccess();
  }

  void move_to_back(K const &k) {
    copy_guard guard(&queuePtr, &enforceCopy);
    queuePtr->move_to_back(k);
    guard.setSuccess();
  }

  std::pair<K const &, V &> front() {
    copy_guard guard(&queuePtr, &enforceCopy, true);
    auto ret = queuePtr->front();
    guard.setSuccess();
    return ret;
  }

  std::pair<K const &, V &> back() {
    copy_guard guard(&queuePtr, &enforceCopy, true);
    auto ret = queuePtr->back();
    guard.setSuccess();
    return ret;
  }

  std::pair<K const &, V const &> front() const {
    return queuePtr->front();
  }

  std::pair<K const &, V const &> back() const {
    return queuePtr->back();
  }

  std::pair<K const &, V &> first(K const &key) {
    copy_guard guard(&queuePtr, &enforceCopy, true);
    auto ret = queuePtr->first(key);
    guard.setSuccess();
    return ret;
  }

  std::pair<K const &, V &> last(K const &key) {
    copy_guard guard(&queuePtr, &enforceCopy, true);
    auto ret = queuePtr->last(key);
    guard.setSuccess();
    return ret;
  }

  std::pair<K const &, V const &> first(K const &key) const {
    return queuePtr->first(key);
  }

  std::pair<K const &, V const &> last(K const &key) const {
    return queuePtr->last(key);
  }

  k_iterator k_begin() {
    return queuePtr->k_begin();
  }

  k_iterator k_end() {
    return queuePtr->k_end();
  }

  void clear() noexcept {
    enforceCopy = false;
    queuePtr->clear();
  }

  size_t size() const noexcept {
    return queuePtr->elemList.size();
  }

  bool empty() const noexcept {
    return queuePtr->elemList.size() == 0;
  }

  size_t count(K const &k) const {
    return queuePtr->count(k);
  }
};

template <class K, class V>
class keyed_queue<K, V>::copy_guard {
private:
  bool success = false;
  bool oldEnforceCopy;
  bool *enforceCopyPtr;
  std::shared_ptr<keyed_queue_impl> old;
  std::shared_ptr<keyed_queue_impl> *current;

public:
  ~copy_guard() {
    if (!success && old) {
      *current = old;
    }
    if (!success) {
      *enforceCopyPtr = oldEnforceCopy;
    }
  }

  copy_guard(std::shared_ptr<keyed_queue_impl> *shr_ptr, bool *_enforceCopyPtr,
             bool setEnforceCopy = false) {
    current = shr_ptr;
    enforceCopyPtr = _enforceCopyPtr;
    // Remember old value of enforceCopy and change the value of enforceCopy
    // if needed.
    oldEnforceCopy = *_enforceCopyPtr;
    if (!(current->unique())) {
      keyed_queue_impl kqi(**current);
      *current = std::make_shared<keyed_queue_impl>(kqi);
      // Set enforceCopy again, because after copying it is not dependent
      // on previous reference exposures.
      *enforceCopyPtr = setEnforceCopy;
    } else {
      *enforceCopyPtr |= setEnforceCopy;
    }
  }

  void setSuccess() {
    success = true;
  }
};

template <class K, class V>
class keyed_queue<K, V>::keyed_queue_impl {
  using elem_type = std::pair<K const *, V>;
  using list_iter = typename std::list<std::pair<K const *, V>>::iterator;
  using map_iter = typename std::map<K const, std::list<list_iter>>::iterator;

public:
  class iterator;

  // List containing ordered elements.
  std::list<elem_type> elemList;
  // Map key -> iterator lists, which allows finding key in log n.
  // Order of iterators on list is the same as on elemList.
  std::map<K const, std::list<list_iter>> keyMap;
  keyed_queue_impl() : elemList(), keyMap() {}

  keyed_queue_impl(keyed_queue_impl &other) : elemList(other.elemList) {
    // Iterators in keyMap and pointers on elemList are pointing to the wrong
    // container.
    for (list_iter it = elemList.begin(); it != elemList.end(); ++it) {
      auto keyIters = keyMap.find(*(it->first));
      if (keyIters == keyMap.end()) {
        std::list<list_iter> newList{it};
        auto inserted = keyMap.insert({*(it->first), newList});
        keyIters = inserted.first;
      } else {
        keyIters->second.push_back(it);
      }
      it->first = &keyIters->first;
    }
  }

  // Strong exception guarantee: uses only strong methods and reverses
  // changes in case of an exception.
  void push(K const &k, V const &v) {
    auto key_list_pair = keyMap.find(k);
    if (key_list_pair == keyMap.end()) {
      // map::insert has strong guarantee with a single element.
      // If it fails, there will be no changes in the queue.
      auto inserted = keyMap.insert({k, std::list<list_iter>{}});
      key_list_pair = inserted.first;
    }
    try {
      const K *ptr = &key_list_pair->first;
      std::pair<K const *, V> p(ptr, v);
      std::list<elem_type> l_pair = std::list<elem_type>();
      l_pair.push_back(p);
      elemList.splice(elemList.end(), l_pair); //no-throw
      std::list<list_iter> l_iter;
      try {
        l_iter = std::list<list_iter>();
        l_iter.push_back(--elemList.end());
      } catch (std::exception &e) {
        // Undo the changes when l_iter.push_back() fails.
        elemList.pop_back(); // no-throw
        throw e;
      }
      // no-throw
      key_list_pair->second.splice(key_list_pair->second.end(), l_iter);
    } catch (std::exception &e) {

      if (key_list_pair->second.empty()) {
        keyMap.erase(key_list_pair);
      }
      throw e;
    }
  }

  // Strong except. guarantee: makes changes only at the end with no-throw
  // methods.
  void pop() {
    if (elemList.empty()) {
      throw lookup_error();
    }
    auto keyIters = keyMap.find(*elemList.front().first);

    // All methods below are no-throw.
    // Remove iterator from the set and remove the set if empty in result.
    keyIters->second.pop_front();
    if (keyIters->second.empty()) {
      keyMap.erase(keyIters);
    }
    elemList.pop_front();
  }

  // Strong execpt. guarantee: uses only no-throw methods at the end.
  void pop(K const &k) {
    auto keyIters = keyMap.find(k);
    if (keyIters == keyMap.end()) {
      throw lookup_error();
    }
    bool delete_map_entry = keyIters->second.size() == 1;

    // All methods below are no-throw.
    // Remove the element from elemList
    elemList.erase(keyIters->second.front());
    // Remove iterator from the set and remove the set if empty in result.
    keyIters->second.pop_front();
    if (delete_map_entry) {
      keyMap.erase(keyIters);
    }
  }

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V &> front() {
    if (elemList.empty()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = elemList.front();
    return std::pair<K const &, V &>{*f.first, f.second};
  };

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V &> back() {
    if (elemList.empty()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = elemList.back();
    return std::pair<K const &, V &>{*f.first, f.second};
  };

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V const &> front() const {
    if (elemList.empty()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = elemList.front();
    return std::pair<K const &, V const &>{*f.first, f.second};
  };

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V const &> back() const {
    if (elemList.empty()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = elemList.back();
    return std::pair<K const &, V const &>{*f.first, f.second};
  };

  // Strong except. guarantee: doesn't modify the container.
  size_t count(K const &k) const {
    auto keyIters = keyMap.find(k);
    if (keyIters == keyMap.end()) {
      return 0;
    } else {
      return keyIters->second.size();
    }
  }

  // No throw guarantee: both functions give nothrow guarantee.
  void clear() noexcept {
    elemList.clear();
    keyMap.clear();
  }

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V &> first(K const &key) {
    map_iter m = keyMap.find(key);
    if (m == keyMap.end()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = *m->second.front();
    return std::pair<K const &, V &>{*f.first, f.second};
  }

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V &> last(K const &key) {
    map_iter m = keyMap.find(key);
    if (m == keyMap.end()) {
      throw lookup_error();
    }
    std::pair<K const *, V> &f = *m->second.back();
    return std::pair<K const &, V &>{*f.first, f.second};
  }

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V const &> first(K const &key) const {
    map_iter m = keyMap.find(key);
    if (m == keyMap.end()) {
      throw lookup_error();
    }
    std::pair<K const *, V const> &f = *m->second.front();
    return std::pair<K const &, V const &>{*f.first, f.second};
  }

  // Strong except. guarantee: doesn't modify the container.
  std::pair<K const &, V const &> last(K const &key) const {
    map_iter m = keyMap.find(key);
    if (m == keyMap.end()) {
      throw lookup_error();
    }
    std::pair<K const *, V const> &f = *m->second.back();
    return std::pair<K const &, V const &>{*f.first, f.second};
  }

  // Strong except. guarantee: does not alter the container state.
  iterator k_begin() noexcept {
    return iterator(keyMap.begin());
  }

  // Strong except. guarantee: does not alter the container state.
  iterator k_end() noexcept {
    return iterator(keyMap.end());
  }

  // Strong except. guarantee: It modifies the container only if vector of
  // iterators is correctly computed.
  void move_to_back(K const &k) {
    map_iter m = keyMap.find(k);
    if (m == keyMap.end()) {
      throw lookup_error();
    }

    std::vector<list_iter> temp;
    typename std::list<list_iter>::iterator llit = m->second.begin();
    size_t size = m->second.size();
    temp.resize(size);

    for (size_t current = 0; current < size; current++) {
      temp[current] = *llit;
      ++llit;
    }

    for (size_t current = 0; current < size; current++) {
      elemList.splice(elemList.end(), elemList, temp[current]);
    }

    temp.clear();
  }
};

template <class K, class V>
class keyed_queue<K, V>::keyed_queue_impl::iterator : public map_iter {
public:
  iterator() : map_iter(){};
  iterator(map_iter s) : map_iter(s){};

  K *operator->() {
    return (K * const) & (map_iter::operator->()->first);
  }

  K operator*() {
    return map_iter::operator*().first;
  }
};

#endif // KEYED_QUEUE_H
