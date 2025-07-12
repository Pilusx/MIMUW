#include "alloc.h"
#include <assert.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SLEEP_TIME 0.005

void swap_sort(args_t* a) {
  sem_wait(&a->l->mutex);
  sem_wait(&a->r->mutex);

  if (a->l->val > a->r->val) {
    int min = a->r->val;
    int max = a->l->val;
    a->l->val = min;
    a->r->val = max;

    if (a->left != NULL)
      sem_post(a->left);
    if (a->right != NULL)
      sem_post(a->right);
  }

  sem_post(&a->r->mutex);
  sem_post(&a->l->mutex);
}

void worker(args_t (*func)(G, int), G g, int i) {
  args_t a = (*func)(g, i);
  assert(a.mine != NULL);

  while (true) {
    sem_wait(a.mine);
    if (g->finished)
      break;
    swap_sort(&a);
  }
  exit(0);
}

void myfork(args_t (*func)(G, int), G g, int i) {
  switch (fork()) {
  case 0:
    worker(func, g, i);
    break;
  case -1:
    perror("FORK");
    break;
  default:
    break;
  }
}

void check(G g) {
  int k = 0;
  while (true) {
    sleep(SLEEP_TIME);
    int id = 0;
    lock(g, 0);
    for (; id < 2 * g->n - 1; id++) {
      lock(g, id + 1);
      if (*getv(g, id) > *getv(g, id + 1)) {
        unlock(g, id);
        unlock(g, id + 1);
        break;
      }
      unlock(g, id);
    }
    if (id == 2 * g->n - 1) {
      unlock(g, 2 * g->n - 1);
      break;
    }
    k++;
  }
  // fprintf(stderr, "[FINI] SORTED within %d checks\n", k + 1);
}

#define SHM_NAME "/sort"

int main() {
  int n, global_id;

  G g;
  scanf("%d", &n);
  global_id = init_global(&g, n);

  REP(2 * n) scanf("%d", getv(g, i)); // Wczytaj liczby

  REP(n) myfork(initA, g, i);     // Utworz procesy typu A
  REP(n - 1) myfork(initB, g, i); // Utworz procesy typu B

  check(g); // Sprawdzaj co jakis czas czy tablica jest posortowana
  g->finished = true;
  REP(n) sem_post(&g->a[i]);     // Zakoncz dzialanie procesow typu A
  REP(n - 1) sem_post(&g->b[i]); // Zakończ dzialanie procesow typu B

  REP(2 * n) printf("%d\n", *getv(g, i)); // Wypisz liczby

  detach_global(g); // Wyczysc pamiec wspoldzielona
  shmctl(global_id, IPC_RMID, NULL);
}