#include <iostream>

using namespace std;

int main(){
    cout << "receiver" << endl;
}

/*


Odbiornik odbiera dane wysyłane przez nadajnik i wyprowadza je na standardowe wyjście.
Odbiornik co ok. 5s wysyła na adres DISCOVER_ADDR na port CTRL_PORT prośbę o identyfikację 
(komunikat LOOKUP). Na podstawie otrzymanych odpowiedzi (komunikatów REPLY) 
tworzy listę dostępnych stacji radiowych. Stacja, od której przez 20 sekund 
odbiornik nie otrzymał komunikatu REPLY, jest usuwana z listy. 

Jeśli to była stacja aktualnie odtwarzana, rozpoczyna się odtwarzanie innej stacji. 
Jeśli podano parametr -n, odbiornik rozpoczyna odtwarzanie stacji o zadanej nazwie, 
gdy tylko ją wykryje. Jeśli nie podano argumentu -n, odbiornik rozpoczyna odtwarzanie 
pierwszej wykrytej stacji.
Odbiornik posiada bufor o rozmiarze BSIZE bajtów, przeznaczony do przechowywania danych
 z maksymalnie ⌊BSIZE/PSIZE⌋ kolejnych paczek.
Rozpoczynając odtwarzanie, odbiornik:
Czyści bufor, w szczególności porzucając dane w nim się znajdujące, 
a jeszcze nie wyprowadzone na standardowe wyjście.
Jeśli potrzeba, wypisuje się z poprzedniego adresu grupowego, a zapisuje się na nowy.
Po otrzymaniu pierwszej paczki audio, zapisuje z niej wartość pola session_id oraz 
numer pierwszego odebranego bajtu (nazwijmy go BYTE0; patrz specyfikacja protokołu poniżej), 
oraz rozpoczyna wysyłanie próśb o retransmisję zgodnie z opisem poniżej.
Aż do momentu odebrania bajtu o numerze BYTE0 + ⌊BSIZE*3/4⌋ lub większym, odbiornik nie 
przekazuje danych na standardowe wyjście. Gdy jednak to nastąpi, przekazuje dane
 na standardowe wyjście tak szybko, jak tylko standardowe wyjście na to pozwala.
Powyższą procedurę należy zastosować wszędzie tam, gdzie w treści zadania mowa
jest o rozpoczynaniu odtwarzania.
Jeśli odbiornik miałby wyprowadzić na standardowe wyjście dane, których jednakże 
brakuje w buforze, choćby to była tylko jedna paczka, rozpoczyna odtwarzanie od nowa.
Jeśli odbiornik odbierze nową paczkę, o numerze większym niż dotychczas odebrane, 
umieszcza ją w buforze i w razie potrzeby rezerwuje miejsce na brakujące paczki, 
których miejsce jest przed nią. Jeśli do wykonania tego potrzeba usunąć stare dane, 
które nie zostały jeszcze wyprowadzone na standardowe wyjście, należy to zrobić.
Odbiornik wysyła prośby o retransmisję brakujących paczek. Prośbę o retransmisję paczki 
o numerze n powinien wysłać w momentach t + k*RTIME, gdzie t oznacza moment odebrania 
pierwszej paczki o numerze większym niż n, dla k = 1, 2, … (w nieskończoność, 
póki dana stacja znajduje się na liście dostępnych stacji). Odbiornik nie wysyła próśb o 
retransmisję paczek zawierających bajty wcześniejsze niż BYTE0, ani tak starych, że i tak 
nie będzie na nie miejsca w buforze.
Odbiornik oczekuje połączeń TCP na porcie UI_PORT. Jeśli użytkownik podłączy się tam np. 
programem Telnet, powinien zobaczyć prosty tekstowy interfejs, w którym za pomocą strzałek 
góra/dół można zmieniać stacje (bez konieczności wciskania Entera). 
Oczywiście jeśli jest kilka połączeń, wszystkie powinny wyświetlać to samo, i 
zmiany w jednym z nich powinny być widoczne w drugim. Podobnie, wyświetlana lista stacji 
powinna się aktualizować w przypadku wykrycia nowych stacji lub porzucenia już niedostępnych. 
Powinno to wyglądać dokładnie tak: 
------------------------------------------------------------------------
  SIK Radio
------------------------------------------------------------------------
    PR1
  > Radio "Disco Pruszkow"
    Radiowa Trojka
------------------------------------------------------------------------

Stacje powinny być posortowane alfabetycznie po nazwie. 
Przy każdorazowej zmianie stanu (aktywnej stacji, listy dostępnych stacji itp.) 
listę należy ponownie wyświetlić w całości; w ten sposób będzie można w sposób automatyczny 
przetestować działanie programów.

*/