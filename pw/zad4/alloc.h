#ifndef ALLOC_H
#define ALLOC_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

#define REP(x) for (int i = 0; i < x; i++)

struct mint {
  int val;
  sem_t mutex;
};

typedef struct mint mint_t;
typedef mint_t* mint_ptr_t;

struct args {
  mint_ptr_t l, r;
  int id;
  sem_t *left, *right, *mine;
};

struct global {
  int n;
  bool finished;
  key_t t_key, a_key, b_key;
  int t_id, a_id, b_id;
  mint_ptr_t t;
  sem_t *a, *b;
};

typedef struct args args_t;
typedef struct global global_t;
typedef global_t* G;

mint_ptr_t get(G, int i);
int* getv(G g, int i);
sem_t* getm(G g, int i);

int init_global(G* g, int n);

void detach_global(G g);

args_t initA(G g, int i);

args_t initB(G g, int i);

void lock(G g, int i);

void unlock(G g, int i);

#endif