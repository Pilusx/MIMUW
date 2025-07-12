#ifndef PERSON_H
#define PERSON_H

#include "helper.h"
#include <cassert>
#include <iostream>
#include <memory>

class Adult : public Person {
private:
  Price money;

  class AdultStrategy : public Strategy {
  public:
    void behave(std::ostream& os) const override;
  };

public:
  Adult(const std::string& name, Price money = Price(0))
      : Person(name, std::make_shared<AdultStrategy>()), money(money) {}

  void buyDecoration(Decoration& decoration) {
    money -= decoration.getPrice();
  }

  Price getSavings() const {
    return money;
  }
};

class Child : public Person {
private:
  class ChildStrategy : public Strategy {
  public:
    void behave(std::ostream& os) const override;
  };

public:
  Child(const std::string& name)
      : Person(name, std::make_shared<ChildStrategy>()) {}
};

#endif // PERSON_H
