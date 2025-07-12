#include "alloc.h"
#include <assert.h>
#include <sys/shm.h>
#include <sys/types.h>

mint_ptr_t get(G g, int i) {
  assert(g != NULL);
  assert(g->t != NULL);
  assert(&g->t[i] != NULL);
  return &g->t[i];
}

int* getv(G g, int i) {
  return &g->t[i].val;
}

sem_t* getm(G g, int i) {
  return &g->t[i].mutex;
}

void* shmattach(int id) {
  return shmat(id, NULL, 0);
}

void attach_global(G g) {
  g->t = shmattach(g->t_id);
  g->a = shmattach(g->a_id);
  g->b = shmattach(g->b_id);
}

void detach_global(G g) {
  shmdt(g->a);
  shmdt(g->b);
  shmdt(g->t);

  shmctl(g->a_id, IPC_RMID, NULL);
  shmctl(g->b_id, IPC_RMID, NULL);
  shmctl(g->t_id, IPC_RMID, NULL);
  shmdt(g);
}

void init_semaphores(G g, int n) {
  REP(2 * n) sem_init(getm(g, i), 1, 1);
  REP(n) sem_init(g->a + i, 1, 1);
  REP(n - 1) sem_init(g->b + i, 1, 1);
}

int shmgetid(key_t key, size_t size) {
  return shmget(key, size, IPC_CREAT | 0666);
}

int init_global(G* g_ptr, int n) {
  key_t global_key = ftok(".", 'g');
  int global_id = shmgetid(global_key, sizeof(global_t));

  *g_ptr = shmat(global_id, NULL, 0);
  G g = *g_ptr;

  g->t_id = shmgetid(g->t_key, sizeof(mint_t) * 2 * n);
  g->a_id = shmgetid(g->a_key, sizeof(sem_t) * n);
  g->b_id = shmgetid(g->b_key, sizeof(sem_t) * (n - 1));
  g->n = n;
  g->finished = false;

  attach_global(g);
  init_semaphores(g, n);
  return global_id;
}

args_t initA(G g, int i) {
  args_t a;
  a.l = get(g, 2 * i);
  a.r = get(g, 2 * i + 1);
  a.mine = g->a + i;
  a.left = (i > 0 ? g->b + i - 1 : NULL);
  a.right = (i < g->n - 1 ? g->b + i : NULL);
  a.id = i;
  return a;
}

args_t initB(G g, int i) {
  args_t b;
  b.l = get(g, 2 * i + 1);
  b.r = get(g, 2 * i + 2);
  b.mine = g->b + i;
  b.left = g->a + i;
  b.right = g->a + i + 1;
  b.id = i + g->n;
  return b;
}

void lock(G g, int i) {
  sem_wait(&g->t[i].mutex);
}

void unlock(G g, int i) {
  sem_post(&g->t[i].mutex);
}