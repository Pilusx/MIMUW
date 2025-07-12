#include <iostream>
#include "options.h"


int main(int argc, const char *const *argv){
    sender_options options = sender_get_options(argc, argv);

    std::cout << "sender " + options.name << std::endl;
}

/*
Nadajnik powinien otrzymywać na standardowe wejście strumień danych z taką prędkością, z jaką odbiorcy są w stanie dane przetwarzać, a następnie wysyłać te dane zapakowane w datagramy UDP na port DATA_PORT na wskazany w linii poleceń adres ukierunkowanego rozgłaszania MCAST_ADDR. Dane powinny być przesyłane w paczkach po PSIZE bajtów, zgodnie z protokołem opisanym poniżej.
Nadajnik powinien przechowywać w kolejce FIFO ostatnich FSIZE bajtów przeczytanych z wejścia tak, żeby mógł ponownie wysłać te paczki, o których retransmisję poprosiły odbiorniki.
Nadajnik cały czas zbiera od odbiorników prośby o retransmisje paczek. Gromadzi je przez czas RTIME, następnie wysyła serię retransmisji (podczas ich wysyłania nadal zbiera prośby, do wysłania w kolejnej serii), następnie znów gromadzi je przez czas RTIME, itd.
Nadajnik nasłuchuje na UDP na porcie CTRL_PORT, przyjmując także pakiety rozgłoszeniowe. Powinien rozpoznawać dwa rodzaje komunikatów:
LOOKUP (prośby o identyfikację): na takie natychmiast odpowiada komunikatem REPLY zgodnie ze specyfikacją protokołu poniżej.
REXMIT (prośby o retransmisję paczek): na takie nie odpowiada bezpośrednio; raz na jakiś czas ponownie wysyła paczki, według opisu powyżej.
Po wysłaniu całej zawartości standardowego wejścia nadajnik się kończy z kodem wyjścia 0. Jeśli rozmiar odczytanych danych nie jest podzielny przez PSIZE, ostatnia (niekompletna) paczka jest porzucana; nie jest wysyłana.
*/