#include <iostream>
#include "keyed_queueX.h"

int main () {
    {
        keyed_queue<int, int> kq;
        kq.push(1, 1);
        kq.push(2, 2);
        kq.pop();
        auto f = kq.front();
        f.second++;
        kq.pop();
    }


    int keys[] = {3, 1, 2};

    keyed_queue<int, int> kq1 = (keyed_queue<int, int>) {};

    for (int i = 0; i < 3; ++i) {
        kq1.push(keys[i], i);
    }
    //auto &ref = kq1.front().second;

    keyed_queue<int, int> kq2(kq1); // Wykonuje się pełna kopia, dlaczego?
    keyed_queue<int, int> kq3;

    auto x = kq2.front();
    auto y = kq1.front();
    
   
    kq1.pop(1);
    std::cout << kq1.size() << std::endl;
    
    auto x2 = kq2.front();
    
    auto y2 = kq1.front();
    kq1.pop();
    kq1.pop();
    try {
        kq1.pop();
        std::cout << "FAIL." << std::endl;
    } catch (const lookup_error& le) {
        std::cout << "OK." << std::endl;
    }


//    kq3 = kq2;
}
