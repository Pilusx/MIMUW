#include "helper.h"

int randomNumber() {
  static std::mt19937 mte(0);
  static std::uniform_int_distribution<int> distr(
      0, std::numeric_limits<int>::max());
  return distr(mte);
}

std::ostream& operator<<(std::ostream& os, const Price& p) {
  os << p.price << " PLN";
  return os;
}

Date& currentDate() {
  static Date date = TimeConstants::startDate;
  return date;
}

int operator-(const Date& a, const Date& b) {
  return (a.minutes - b.minutes) +
         TimeConstants::supMinutes *
             (a.hours - b.hours + TimeConstants::supHours * (a.days - b.days));
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
  os << "December, " << d.days << ", " << d.hours << ":"
     << (d.minutes < 10 ? "0" : "") << d.minutes;
  return os;
}

Date& Date::operator++() {
  ++minutes;
  if (minutes == TimeConstants::supMinutes) {
    minutes = TimeConstants::firstMinute;
    ++hours;
  }
  if (hours == TimeConstants::supHours) {
    hours = TimeConstants::firstHour;
    ++days;
  }
  return *this;
}
