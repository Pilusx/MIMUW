#ifndef DECORATION_H
#define DECORATION_H

#include "helper.h"
#include <algorithm>
#include <memory>
#include <string>
#include <utility>

class GlassBall : public Decoration {
private:
  static const int STATE_COUNT = 2, STURDINESS = 4;
  static const std::string stateFormats[STATE_COUNT];
  enum State { OK = 0, BROKEN = 1 };

protected:
  const std::string getState() const override;
  void manipulate() override;

public:
  GlassBall(const std::string& name, Price price) : Decoration(name, price) {}
};

class Lights : public Decoration {
private:
  static const int STATE_COUNT = 3;
  static const std::string stateFormats[STATE_COUNT];

protected:
  const std::string getState() const override;
  void manipulate() override;

public:
  Lights(const std::string& name, Price price) : Decoration(name, price) {}
};

class Tree : public Decoration {
private:
  static const int ENDURANCE = 930, INIT_COVERAGE = 100;
  static const std::string stateSuffix;

protected:
  Date date;
  const std::string getState() const override;
  void manipulate() override;

public:
  Tree(const std::string& name, Price price, Date date)
      : Decoration(name, price, INIT_COVERAGE), date(date) {}
};

class ChristmasTree : public Observable<Observer>,
                      public Composite<Decoration>,
                      public Value {

public:
  ChristmasTree(const std::string& name) : Value(name, Price()) {}

  void addDecoration(std::shared_ptr<Decoration> sd,
                     std::ostream& os = std::cout) {
    addComponent(sd);
    price = price + sd->getPrice();
    notify(os);
    sd->doOperation(os);
  }

  void removeDecoration(Lit it) {
    price -= (*it)->getPrice();
    removeComponent(it);
  }

  void attachObserver(std::shared_ptr<Observer> observer) {
    addObserver(observer);
    observer->setTotal(&price);
  }

  void detachObserver(std::shared_ptr<Observer> observer) {
    delObserver(observer);
    observer->resetTotal();
  }

  using Composite::begin;

  using Composite::end;
};

#endif // DECORATION_H
