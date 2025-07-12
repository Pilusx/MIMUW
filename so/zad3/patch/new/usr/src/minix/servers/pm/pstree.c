#include <stdio.h>
#include <stdlib.h>
#include "pm.h"
#include "mproc.h"

#define parent(i) mproc[(i)].mp_parent
#define user(i) mproc[(i)].mp_realuid
#define pid(i) mproc[(i)].mp_pid
#define gid(i) mproc[(i)].mp_realgid
#define active(i) (mproc[(i)].mp_flags & IN_USE)
#define important(i) (active(i) && user(i) == uid && gid(i) == rgid)


void sort(int* array, int num, int(*cmp)(const int* a, const int *b)) {
    int i, j, temp;
    for(i = 0; i < num; i++) {
        for(j = 0; j < num - 1; j++) {
            if(cmp(array + j, array + j + 1) > 0) {
                temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
            }
        }
    }
}

void print_pid(int idx, unsigned level) {
  int l;
  for (l = 0; l < level; l++) printf("---");
  printf("%d\n", pid(idx));
}

int process_cmp(const int* a, const int* b) {
  int av = pid(*(int*)a);
  int bv = pid(*(int*)b);
  return av - bv;
}

void print_sons(int p_idx, const int uid, const uid_t rgid, unsigned level) {
#define son_condition(i) \
  (important(i) && parent(i) == p_idx && pid(parent(i)) != pid(i) && gid(i) == rgid)

  print_pid(p_idx, level);
  int i, counter = 0;
  size_t sons_num = 0;
  for (i = 0; i < NR_PROCS; i++)
    if (son_condition(i)) sons_num++;

  int* sons = malloc(sizeof(int) * sons_num);

  for (i = 0; i < NR_PROCS; i++)
    if (son_condition(i) && counter < sons_num) {
      sons[counter] = i;
      counter++;
    }

  sort(sons, sons_num, process_cmp);

  for (i = 0; i < sons_num; i++)
    if (son_condition(sons[i])) print_sons(sons[i], uid, rgid, level + 1);

  free(sons);
}

int do_pstree(void) {
  pid_t pid = m_in.m1_i1;
  int uid = m_in.m1_i2;
  gid_t rgid = m_in.m1_i3;

  int i = 0;
  for (i = 0; i < NR_PROCS; i++)
    if (important(i) && pid(i) == pid) print_sons(i, uid, rgid, 0);

  return OK;
}
