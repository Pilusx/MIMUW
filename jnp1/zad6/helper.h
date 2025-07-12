#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <set>

int randomNumber();

class Price {
private:
  int price;

public:
  Price(int price = 0) : price(price) {}

  Price operator+(const Price& other) const {
    return Price(this->price + other.price);
  }

  Price& operator-=(const Price& other) {
    price -= other.price;
    return *this;
  }

  bool operator<(const Price& other) const {
    return price < other.price;
  }

  friend std::ostream& operator<<(std::ostream& os, const Price& p);
};

class Nameable {
private:
  const std::string name;

protected:
  Nameable(const std::string& name) : name(name) {}

public:
  const std::string& getName() const {
    return name;
  }
};

class Valuable {
protected:
  Price price;
  Valuable(Price price) : price(price) {}

public:
  Price getPrice() const {
    return price;
  }
};

class Value : public Nameable, public Valuable {
protected:
  Value(const std::string& name, Price price)
      : Nameable(name), Valuable(price) {}
};

class Operatable {
public:
  virtual void doOperation(std::ostream& os) = 0;
};

class Strategy {
protected:
  // assumes strategy user observes only one object at a time
  Price* totalPrice = nullptr;

public:
  void setTotal(Price* t) {
    totalPrice = t;
  }

  void resetTotal() {
    totalPrice = nullptr;
  }

  virtual void behave(std::ostream& os) const = 0;
  virtual ~Strategy() {}
};

class Observer {
private:
  std::shared_ptr<Strategy> strategy;

protected:
  Observer(std::shared_ptr<Strategy> strategy) : strategy(strategy) {}

public:
  void setTotal(Price* t = nullptr) {
    strategy->setTotal(t);
  }

  void resetTotal() {
    strategy->resetTotal();
  }

  void update(std::ostream& os = std::cout) const {
    strategy->behave(os);
  }
};

class Decoration : public Value, public Operatable {
protected:
  int state = 0;

  Decoration(const std::string& name, Price price, int state = 0)
      : Value(name, price), state(state) {}

  virtual const std::string getState() const = 0;
  virtual void manipulate() = 0;

  void printStatus(std::ostream& os) const {
    os << getName() << ": " << getState() << "\n";
  }

public:
  virtual ~Decoration() {}

  void doOperation(std::ostream& os) {
    manipulate();
    printStatus(os);
  }
};

class Person : public Nameable, public Observer {
protected:
  Person(const std::string& name, std::shared_ptr<Strategy> strategy)
      : Nameable(name), Observer(strategy) {}
};

template <class T>
class Observable : private std::set<std::shared_ptr<T>> {
  static_assert(std::is_convertible<T*, Observer*>::value,
                "Elements must be observers");

protected:
  void addObserver(std::shared_ptr<T> observer) {
    this->insert(observer);
  }

  void delObserver(std::shared_ptr<T> observer) {
    this->erase(this->find(observer));
  }

  Observable() {}

public:
  void notify(std::ostream& os = std::cout) const {
    for (auto& e : *this)
      e->update(os);
  }
};

template <class T>
class Composite : protected std::list<std::shared_ptr<T>>, public Operatable {
  static_assert(std::is_convertible<T*, Operatable*>::value,
                "Elements must be operatable");

protected:
  using Lit = typename std::list<std::shared_ptr<T>>::iterator;

  void addComponent(std::shared_ptr<T> t) {
    this->push_back(t);
  }

  void removeComponent(Lit it) {
    this->erase(it);
  }

  Composite() {}

public:
  void doOperation(std::ostream& os) {
    for (auto& e : *this)
      e->doOperation(os);
  }
};

class Date {
private:
  int days;
  int hours;
  int minutes;

public:
  Date(int days, int hours, int minutes)
      : days(days), hours(hours), minutes(minutes) {}

  Date& operator++();
  friend std::ostream& operator<<(std::ostream& os, const Date& d);
  friend int operator-(const Date& a, const Date& b);

  bool operator<(const Date& other) const {
    return operator-(*this, other) < 0;
  }
};

namespace TimeConstants {
static const Date startDate = Date(18, 16, 0);
static const int supHours = 24; // 0..23
static const int firstHour = 0;
static const int supMinutes = 60; // 0..59
static const int firstMinute = 0;
}

int operator-(const Date& a, const Date& b);
Date& currentDate();

#endif // HELPER_H
