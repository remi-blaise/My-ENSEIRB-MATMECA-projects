#include <stdio.h>
#include <assert.h>
#include "../include/thread.h"

/* test du join du main par un fils.
 *
 * le programme doit terminer correctement (faire le printf et s'arreter entièrement).
 * valgrind doit être content.
 *
 * support nécessaire:
 * - thread_create()
 * - thread_self() dans le main
 * - thread_exit() dans le main
 * - thread_join() du main par un autre thread
 */

thread_t thmain = NULL;

static void * thfunc(void *dummy __attribute__((unused)))
{
  void *res;
  int err;

  err = thread_join(thmain, &res);
  assert(!err);
  assert(res == (void*) 0xdeadbeef);
  printf("main terminé OK\n");
  return NULL;
}

int main()
{
  thread_t th;
  int err;

  thmain = thread_self();

  err = thread_create(&th, thfunc, NULL);
  assert(!err);

  thread_exit((void*) 0xdeadbeef);
  return 0; /* unreachable, shut up the compiler */
}
