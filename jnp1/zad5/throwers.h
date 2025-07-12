#include "keyed_queue.h"
#include <cassert>
#include <iostream>

static int counter;

class Thrower {
protected:
  int value;
  void tryThrow() const {
    counter--;
    if (counter == -1) { // was 0
      // std::cout << "Throwing!\n";
      throw "exception()";
    }
  }
  Thrower(int ctr, int val) : value(val) { counter = ctr; tryThrow(); }
  Thrower(int val) : value(val) {tryThrow();}

};

class ThrowerKey : private Thrower {
public:
  ThrowerKey(int ctr, int val) : Thrower(ctr, val) {}
  ThrowerKey() : ThrowerKey(42) {}
  ThrowerKey(int val) : Thrower(val) {}
  ThrowerKey(ThrowerKey const &o) : Thrower(counter, o.value) {}
  ThrowerKey(ThrowerKey &&o) : Thrower(counter, o.value) {}
  void operator &= (ThrowerKey const &o) {
    value = o.value;
    tryThrow();
  }
  ThrowerKey& operator=(ThrowerKey const&& rhs) = delete;

  bool operator<(ThrowerKey const& rhs) const {
    tryThrow();
    return this->value < rhs.value;
  }

  int &get() {
    return value;
  }
  int const &get() const {
    return value;
  }
};

class ThrowerVal : private Thrower {
public:
  ThrowerVal(int ctr, int val) : Thrower(ctr, val) {}
  ThrowerVal(ThrowerVal const &o) : Thrower(counter, o.value) {}
  ThrowerVal(int val) : Thrower(val) {}
  ThrowerVal() = delete;
  ThrowerVal(ThrowerVal &&o) = delete;
  ThrowerVal& operator=(ThrowerVal const&& rhs) = delete;
  int &get() {
    return value;
  }
  int const &get() const {
    return value;
  }

  void customSetter(int a) {
    value = a;
  }
};


class testKQ {
private:
  keyed_queue<ThrowerKey, ThrowerVal> b;
  keyed_queue<int, int&> c;
  using K = ThrowerKey;
  using V = ThrowerVal;
  using KQiter = keyed_queue<K, V>::k_iterator;

public:
  testKQ() : b(), c() {}

  testKQ(testKQ const &p) : b(p.b), c(p.c) {}

  testKQ(testKQ &&p) : b(p.b), c(p.c) {}

  testKQ &operator=(testKQ &other) {
    b = other.b;
    c = other.c;
    return *this;
  }

  void push(int const &k, int const &v) {
    b.push(ThrowerKey(k), ThrowerVal(v));
    c.push(b.back().first.get(), b.back().second.get());
  };

  void pop() {
    b.pop();
    c.pop();
  };

  void pop(K const &k){
    b.pop(k);
    c.pop(k.get());
  };

  void move_to_back(K const &k) {
    b.move_to_back(k);
    c.move_to_back(k.get());
  };

  std::pair<K const &, V &> front() {
    return b.front();
  };
  std::pair<K const &, V &> back() {
    return b.back();
  };
  std::pair<K const &, V const &> front() const {
    return b.front();
  };
  std::pair<K const &, V const &> back() const {
    return b.back();
  };

  std::pair<K const &, V &> first(K const &key) {
    return b.first(key);
  };
  std::pair<K const &, V &> last(K const &key) {
    return b.last(key);
  };
  std::pair<K const &, V const &> first(K const &key) const {
    return b.first(key);
  };
  std::pair<K const &, V const &> last(K const &key) const {
    return b.last(key);
  };

  size_t size() const noexcept {
    return b.size();
  };

  bool empty() const noexcept {
    return b.empty();
  };

  void clear() {
    b.clear();
    c.clear();
  }

  size_t count(ThrowerKey const &k) const noexcept {
    return b.count(k);
  };

  KQiter k_begin() {
    return b.k_begin();
  }

  KQiter k_end() {
    return b.k_end();
  }

  void compare() {
    while (!b.empty() && !c.empty()) {
      assert(b.front().second.get() == c.front().second);
      b.pop();
      c.pop();
    }
    assert (b.empty() && c.empty());
  }
};
