#include <iostream>
#include "helper.h"
#include "decoration.h"
#include "person.h"

using namespace std;

int main()
{
  // Rodzina Kowalskich...
  auto John = make_shared<Adult>("Jan", Price(1000));
  auto Mary = make_shared<Adult>("Marysia");
  auto Ada = make_shared<Child>("Ada");

  // ... chciała mieć świąteczne drzewko...
  ChristmasTree tree("Christmas tree");

  // ... więc postanowiła kupić świeży świerk...
  auto spruce = make_shared<Tree>("Fresh spruce", Price(300), Date(18, 8, 10));

  // ... i różne ozdoby,
  // światełka za 150 zł
  auto lights = make_shared<Lights>("Bright colorful lights withs 100 LEDs",
                                    Price(150));
  // i kilka bombek.
  auto glassBall1 = make_shared<GlassBall>("Glass ball in shape of snowman",
                                           Price(10));
  auto glassBall2 = make_shared<GlassBall>("Glass ball with Christmas tree",
                                           Price(15));
  auto glassBall3 = make_shared<GlassBall>("Glass ball with presents",
                                           Price(15));

  // Za zakupy odpowiedzialny jest Jan. Żona i córka obserwują pojawianie się
  // ozdób:
  tree.attachObserver(Mary);
  tree.attachObserver(Ada);

  // Podczas gdy Marysia z Adą ozdabiają drzewko...
  tree.addDecoration(spruce);
  tree.addDecoration(lights);
  tree.addDecoration(glassBall1);
  tree.addDecoration(glassBall2);
  tree.addDecoration(glassBall3);

  // Jan płaci rachunek kurierowi.
  auto savings0 = John->getSavings();
  for (auto& x : tree) {
    John->buyDecoration(*x);
  }
  
  auto savings1 = John->getSavings();

  cout << "Savings before: " << savings0 << " and after: " << savings1 << ".\n";
  // Drzewko ozdobione. Rachunek zapłacony. Od tego momentu startuje zegar.
  for (auto& time = currentDate();
       time < Date(24, 18, 0);
       ++time) {
    // Ada bawi się ozdobami choinkowymi o losowych porach (również nocą)...
    if (randomNumber() % 200 == 0) {
      cout << time << "\n";
      for (auto& x : tree) {
        // ... przyglądając się i manipulując losowo wybranymi ozdobami.
        if (randomNumber() % 4 == 0) {
          x->doOperation(cout);
        }
      }
    }
  }
}
