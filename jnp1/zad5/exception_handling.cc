#include "throwers.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <utility>

auto f(testKQ q)
{
  return q;
}

int main () {

  testKQ kq;

  for (int i = 0; i < 100; ++i) {
    counter = i;
    std::unique_ptr<testKQ> kq1, kq2, kq3;
    std::unique_ptr<testKQ const> kq4;

    try {
      int keys[] = {3, 1, 2};

      kq1 = std::make_unique<testKQ>(f({}));

      for (int i = 0; i < 3; ++i) {
        kq1->push(keys[i], i);
      }

      auto &ref = kq1->front().second;

      kq2 = std::make_unique<testKQ>(*kq1); // Wykonuje się pełna kopia, dlaczego?
      kq3 = std::make_unique<testKQ>();
      *kq3 = *kq2;

      ref.customSetter(10);
      assert(kq1->front().second.get() == 10);
      assert(kq2->front().second.get() != 10);

      kq2->pop(); // Obiekt kq2 dokonuje kopii i przestaje współdzielić dane z kq3.
      assert(kq2->size() == 2);
      assert(kq2->count(3) == 0);
      assert(kq2->count(2) == 1);

      assert(kq3->size() == 3);
      assert(kq3->count(3) == 1);

      kq2->push(1, 3);
      kq2->move_to_back(1);
      assert(kq2->size() == 3);
      assert(kq2->front().second.get() == 2
             && kq2->first(1).second.get() == 1
             && kq2->last(1).second.get() == 3
             && kq2->back().second.get() == 3);

      // keyed_queue<int, int> const kq4 = kq2;
      // assert(kq4.front().second == 2
      //        && kq4.first(1).second == 1
      //        && kq4.last(1).second == 3
      //        && kq4.back().second == 3);

      int i = 1;
      for (auto k_it = kq1->k_begin(), k_end = kq1->k_end();
           k_it != k_end;
           ++k_it, ++i) {
        assert(i <= 3 && (*k_it).get() == i);
      }

      // auto kq5 = std::make_unique<keyed_queue<int, int>>();
      // kq5->push(4, 0);
      // assert(kq5->front().first == 4 && kq5->front().second == 0);
      // auto kq6(*kq5);
      // kq5.reset();
      // assert(kq6.front().first == 4 && kq6.front().second == 0);
      //
      // std::swap(kq1, kq2);
      // std::vector<keyed_queue<int, int>> vec;
      // for (int i = 0; i < 100000; i++) {
      //   kq1.push(i, i);
      // }
      // for (int i = 0; i < 1000000; i++) {
      //   vec.push_back(kq1);  // Wszystkie obiekty w vec współdzielą dane.
      // }
    }
    catch (char const *e) {
      // std::cout << "blah\n";
      if (kq1) kq1->compare();
      if (kq2) kq2->compare();
      if (kq3) kq3->compare();
    }
    std::cout << "Passes " << i << "-th exception's test\n";
  }

  std::cout << "OK!\n";
  return 0;
}
