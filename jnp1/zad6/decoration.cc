#include "decoration.h"

const std::string GlassBall::stateFormats[] = {"OK", "broken"};
const std::string Lights::stateFormats[] = {"off", "constant", "blinking"};
const std::string Tree::stateSuffix = "% of tree covered with needles";

const std::string GlassBall::getState() const {
  return stateFormats[state % STATE_COUNT];
}

void GlassBall::manipulate() {
  if (state == State::OK && randomNumber() % STURDINESS == 0)
    state = State::BROKEN;
}

const std::string Lights::getState() const {
  return stateFormats[state % STATE_COUNT];
}

void Lights::manipulate() {
  state = (state + 1) % STATE_COUNT;
}

const std::string Tree::getState() const {
  return std::to_string(state) + stateSuffix;
}

void Tree::manipulate() {
  state =
      INIT_COVERAGE - ((currentDate() - TimeConstants::startDate) / ENDURANCE);
}
