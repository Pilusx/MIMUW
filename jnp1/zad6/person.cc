#include "person.h"
#include "helper.h"

void Adult::AdultStrategy::behave(std::ostream& os) const {
  assert(totalPrice != nullptr);
  os << "We have new decoration!\nTotal cost is " << *totalPrice << ".\n";
}

void Child::ChildStrategy::behave(std::ostream& os) const {
  os << "Wow!\n";
}
